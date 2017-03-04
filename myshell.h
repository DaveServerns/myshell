#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>



void print_cmd_prompt(){
  /*get the current path
    print the path (cwd)*/
    char buf[MAXPATHLEN];
    printf("%s@/MyShell:%s=:> ", getenv("USER"), getcwd(buf,sizeof(buf)));
}

char** get_cmd_args(char* str){
  /*tokenize the input from command line and store in a point to pointers and
    return to main method to be run through the eval method*/
    //tokens will be our return of each tokenized string
    unsigned int bufSize = MAXPATHLEN;
    int index=0;
    char** tokens = malloc(bufSize * sizeof(char *));
    char* token;

    if(!tokens){
      fprintf(stderr, "%s\n","Allocation error, seg faults are no fun");
      exit(EXIT_FAILURE);
    }

    //use strtok() to grab strings from line ignoring delim " "
    token = strtok(str, " ,\n,&");

    //add each token string to the  **tokens
    while(token != NULL){
      tokens[index] = token;
      index++;

      //check to see if we need to increase memory Allocation
      if(index >= bufSize){
        bufSize += MAXPATHLEN;
        tokens = realloc(tokens, bufSize * sizeof(char*));
        if(!tokens){
          fprintf(stderr, "%s\n","Allocation error, seg faults are no fun");
          exit(EXIT_FAILURE);
        }
      }
      token = strtok(NULL, " ,\n,&");

    }
    //set end of tokens to NULL
    tokens[index]= NULL;
    return tokens;

}

/*will run when "clr" is entered, for now just prints \n char to clear the screen
  run loop 100 times clears screen even in maxim mode*/
void clear_shell(){
/*  int s;
  for(s=0;s<100;s++)
  {
    printf("\n");
  }*/
  //found this line of code to clear terminal on the internt
  //above is my original solution but this is much cleaner
  printf("\33[2J\33[1;1H\n" );
}

/*change_dir will take the the path given by the user and
point to that as the current work dir*/
void change_dir(int argc,char** inputs){
  char* dir = inputs[1];


  if(argc<2)
  {
    chdir("/home");
  }
  else
  {
    char buf[MAXPATHLEN];
    getcwd(buf,sizeof(buf));
    strcat(buf,"/");
    strcat(buf,dir);

    int ret;

    ret =chdir(buf);


    if(ret<0)
    {
      printf("%s\n", "This is not the directory you are looking for..." );
    }
  }


}

/*show_dir will print the contents of the cwd dir to the screen it is envoked when the user
enters "dir" as the cmd
*/
void show_dir(){


DIR *dir;
struct dirent *ent;
char buf[MAXPATHLEN];
if ((dir = opendir (getcwd(buf, sizeof(buf)))) != NULL) {
  /* print all the files and directories within directory */
  while ((ent = readdir (dir)) != NULL) {
    printf ("%s\n", ent->d_name);
    }
closedir (dir);
  }
else {
  /* could not open directory */
  perror ("");
  printf("%d\n",EXIT_FAILURE );
  }
}

/*The echo command will print the argument to the screen*/
void echo_cmd(int argc, char** inputs){
  if(argc<2)
  {
    printf("\n" );
  }
  else
  {
    int i;
    for(i=1; inputs[i]!='\0';i++)
    {
        printf("%s ",inputs[i] );
    }
    printf("\n");
  }

}

/*The print_environ method will will print the environment varibles to the terminal
  when envoked by "environ" command*/

void print_environ() {
  extern char** environ;
  int n = 0;
  while(environ[n])
  {
    printf("%s\n",environ[n++] );
  }
}

/*if the command is not a built in function the shell will
  attempt to exec the program*/
int fork_exec(int bg,int argc,char** inputs){
  //pid for the child process (the exec'd command)
  pid_t cp;
  int flag;
  int status;

  //get the args for what will be exec'd
  //just some real simple string manipulation
  //all strings in inputs[1-n] places into single
  //string
/*  char* my_argv;
  int i,j;
  j=1;
  for(i=0,i<argc,i++)
  {
    if(i==(argc-1))
    {
      my_argv[i]='\0';
    }
    else if(i==0)
    {
      my_argv[i]=inputs[j];
      j++;
    }
    else
    {
      my_argv[i]=strcat(" ",inputs[j];)
      j++;
    }
  }*/

  if(cp=fork() == -1)
  {
    printf("%s\n","fork() failed :(" );
    return(-1);
  }
  switch (cp) {
    case 0:
    {
      flag= execvp(inputs[0], inputs);
      break;
    }
    default:
    {
      if(!bg)
      {
        wait(&status);
      }

      break;

    }

  }


  return flag;
}

/*this function takes the original char stream and checks to
see if the operation runs in the background*/
int is_bg(char* stream){
  int i;
  int bg_flag=0;
  for(i=0;stream[i]!='\0';i++)
  {
    if(stream[i]=='&')
    {
      bg_flag=1;
    }
  }
  return bg_flag;
}

/*need to check for redirect symbols in cmd stream this will check for
">", ">>", and "<" symbols and returns a flag to determine if they are present
and if they are, which one is envoked
*/
int check_redir(char** inputs){

  int i, r_flag;
  i=0;
  r_flag=0; //zero by default to denote no redirects

  while(inputs[i] != '\0')
  {
    if (strcmp(inputs[i],">")==0)
    {
      r_flag=1;
    }
    else if(strcmp(inputs[i],">>"))
    {
      r_flag=2;
    }
    else if(strcmp(inputs[i],"<"))
    {
      r_flag=3;
    }
    else
    {
      continue;
    }
    i++;
  }
  return r_flag;
}

/*based on the flag and type of redirection this fucntions*/
void redirection(char* input, int flag){
    int in, out; //file descriptos for input or output


    if(flag ==1)//if flag is 1 that means the redirection is > which means writing
      {      //output of program to the file changing stdout fd
        out = open(input, O_WRONLY | O_TRUNC | O_CREAT,
                  S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        if(out<0)
        {
          //open failed print error
          printf("error opening file %s\n",strerror(errno));
        }
        dup2(out,STDOUT_FILENO);//changes the old fd out with the new one making
                                //stdoout point to input
        close(out);

      }
      else if(flag==2)//if flag is 2 that means the redirection is >> which means writing
      {      //output of program to the file changing stdout fd only this will appen
            //if the file exists
        out = open(input, O_APPEND | O_CREAT,
                  S_IRWXU);
        if(out<0)
        {
          //open failed print error
          printf("error opening file %s\n",strerror(errno));
        }
        dup2(out,STDOUT_FILENO);//changes the old fd out with the new one making
                                //stdoout point to input
        close(out);

      }
      else if(flag==3) /* this means the symbol is < which redirects std from a files
              to a program */
      {
        in = open(input, O_RDONLY);//give file and permission for read only to in
        if(in<0)
        {
          printf("error opening file %s\n",strerror(errno));
        }
        dup2(in, STDIN_FILENO);
        close(in);


      }
      else
        printf("%s\n","big trouble is little myshell" );

    }


/* eval will read the args that have been tokenizes and perfrom the appropriate builin function
if no function exists it will run the command as an exec
*/
void eval(int bg,char** inputs){

    //determine amount of arguments passed to cmd line
    int argc=0;
    int i,rd; //rd used for redirects
    for(i=0; inputs[i]!='\0';i++)
    {
      argc++;
    }
    //if there are not args entered the fucn will return and then print the prompt to get
    //another set of commands
    if(argc == 0)
    {
      return;
    }

    //check for redirection
    if(rd=check_redir(inputs))
    {
      redirection(inputs[2],rd);
    }
    printf("%s\n","I make it here" );

    char* cmd_1 = inputs[0];

    //evaluate the command and perform the appropriate Allocation
    if(strcmp(cmd_1,"clr")==0)
    {
      clear_shell();
    }
    else if(strcmp(cmd_1,"cd")==0)
    {
      change_dir(argc,inputs);
    }
    else if(strcmp(cmd_1, "dir")==0)
    {
      show_dir();
    }
    else if(strcmp(cmd_1,"echo")==0)
    {
      echo_cmd(argc,inputs);
    }
    else if(strcmp(cmd_1,"environ")==0)
    {
      print_environ();
    }
    else
    {
      int ran = fork_exec(bg,argc,inputs);
      if (!ran)
      {
        printf("%s\n", "No such command...try again");

      }

    }

    //free(cmd_1);

}
