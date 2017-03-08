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
void fork_exec(int bg,int argc,char** inputs){
  //pid for the child process (the exec'd command)
  pid_t cp;

  int status;



  if((cp=fork())==0)
  {
    // in the child
    if(execvp(inputs[0],inputs)==-1)
    {
      char* err = inputs[0];
      perror(err);
    }
    exit(EXIT_FAILURE);
  }
  else
  {
    if(bg>0) // check to see if a background process
    {
      printf("[ %d ] %s\n",cp, inputs[0]);
      return; //if it is dont wait for child just return.
    }
    else
    {
      do{//parent
      //{
        waitpid(cp, &status, WUNTRACED);
     }while(!WIFEXITED(status)&&!WIFSIGNALED(status));

    }




  }

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

  int i, flag;
  i=0;
  flag=0; //zero by default to denote no redirects

  while(inputs[i] != '\0')
  {
    
    if (strcmp(inputs[i],">")==0)
    {
      flag=1;
    }
    else if(strcmp(inputs[i],">>")==0)
    {
      flag=2;
    }
    else if(strcmp(inputs[i],"<")==0)
    {
      flag=3;
    }

    i++;
  }

  return flag;
}

/*check redir position in conjunction with the redir flag this will
determine where the symbol is in the tokenized string*/
int check_redir_pos(char** inputs){

  int i, flag;
  i=0;
  flag=0; //zero by default to denote no redirects

  /*run through the tokens and when redirect is found
   return that position will use this to determine where the file
   is and what is being redirected to
   it*/
  while(inputs[i] != '\0')
  {

    if (strcmp(inputs[i],">")==0)
    {
      return flag=i;
    }
    else if(strcmp(inputs[i],">>")==0)
    {
      return flag=i;
    }
    else if(strcmp(inputs[i],"<")==0)
    {
      return flag=i;
    }

    i++;
  }

  return flag;
}

/*based on the flag and type of redirection this fucntions*/
void redirection(char* input, int flag){
    int in, out; //file descriptos for input or output
    //int saved_stdin, saved_stdout; //temp storage for the default stdin and out

    if(flag ==1)//if flag is 1 that means the redirection is > which means writing
      {      //output of program to the file changing stdout fd
        out = open(input, O_WRONLY  | O_CREAT | O_TRUNC, S_IRWXU);
        if(out<0)
        {
          //open failed print error
          printf("error opening file %s\n",strerror(errno));
        }

        dup2(out,1);//changes the old fd out with the new one making
                                //stdoout point to input
        close(out);



      }
      else if(flag==2)//if flag is 2 that means the redirection is >> which means writing
      {      //output of program to the file changing stdout fd only this will appen
            //if the file exists

        out = open(input, O_APPEND | O_CREAT| O_WRONLY);
        if(out<0)
        {
          //open failed print error
          printf("error opening file %s\n",strerror(errno));
        }
        dup2(out,1);//changes the old fd out with the new one making
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
        dup2(in, 0);
        close(in);


      }
      else
        printf("%s\n","big trouble is little myshell" );

    }

    /*help_me opens the help file for the user and hopefully uses the more
    filter to display it one page at a time*/
    void help_me(){
      system("more readme");
    }

    int is_pipe(char** inputs){
      int j; //finding the pipe in loop
      int pipePresent = 0; //do we need to pipe?
      for(j=0; inputs[j]!='\0'; j++)
      {
        if(strcmp(inputs[j],"|")==0)
        {
          pipePresent =j;
        }
      }
      return pipePresent;
    }

    /*the my_pipe function is to process two commands that allow the processes called be these
    commands to communicate the input output steams*/
    void my_pipe(char** readEnd, char** writeEnd){
      pid_t cpid;
      int fd[2]; //file descriptor for the pipe either 0 or 1



      if (pipe(fd) == -1) {
    		perror("pipe");
    	}

    	//Fork process to run second cmd
    	if (fork() == 0)	{						// First Child running 2nd cmd with new stdin
        close(fd[1]);
        dup2(fd[0], 0);							// Setting stdin to read end of pipe
    		execvp(readEnd[0],readEnd);

    	}

    	else if ((cpid = fork()) == 0){				// Second child running cmd1 with new stdout
        close(fd[0]);
        dup2(fd[1], 1);							// Setting stdout to write end of pipe
    		execvp(writeEnd[0],writeEnd);


    	}

    	else {
    		dup2(1, fd[1]);
    		fflush(stdout);
    		waitpid(cpid, NULL, 0);
    	}
    }

    void eval(int bg,char** inputs){//evaluate the user input and perform the action


        //determine amount of arguments passed to cmd line
        int argc=0;
        int i; //rd used for redirects
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
        int rd;
        int saved_stdout, saved_stdin; //store original stdin and out
        //check for redirection
        char** ex_args; // hold the tokens before redirect (ie "ls -l > file.txt" will store in ex_args)
        int k; // for the loop
        if(rd = check_redir(inputs))
        {


          int redir = check_redir_pos(inputs);// redir is the position of the redirect symbol is in inputs
          char *file = inputs[redir + 1]; // sets file equal to the tok after the redirect symbol

          for(k=0;k < (redir-1);k++)
          {
            ex_args[k]=inputs[k];
          }
          /*saving the state of stdin and out so that
            after the redir we can return input to keyboard and
            output to screen*/

          if (rd==1 || rd == 2) {
            saved_stdout = dup(STDOUT_FILENO);
            redirection(file,rd);
          }
          else if(rd==3)
          {
            saved_stdin = dup(STDIN_FILENO);
            redirection(file,rd);
          }


        }

        //search for pipe symbol and then determine its position
        /*int pipePos= is_pipe(inputs);
        //if pipePos then there is a pipe and we need to break up the read end of
        //the pipe and write end
        char** cmdRead;
        char** cmdWrite;
        if(pipePos>0)
        {
          cmdRead[0] = inputs[pipePos-1];
          cmdWrite[0] = inputs[pipePos+1];
        }



        char* cmd_1 = inputs[0];

        if(pipePos>0)
        {
          my_pipe(cmdRead,cmdWrite);
        }*/
        //evaluate the command and perform the appropriate Allocation
        char* cmd_1 = inputs[0];
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
        else if(strcmp(cmd_1,"help")==0)
        {
          help_me();
        }
        else
        {

          if(rd)
          {

            /*if their is a redirect need to pass the new string to it in exec
            excluding the > >> < symbol and the file */
            fork_exec(bg,argc,ex_args);
          }
          else{
            fork_exec(bg,argc,inputs);
          }


          /*if (ran < 0)
          {
            printf("%s\n", "No such command...try again");

          }
          printf("wpid: %d",ran);*/


        }

      //free(cmd_1);
      //restore the std in out back to the saved state.
      if (rd==1 || rd == 2)
      {
        dup2(saved_stdout,1);
        close(saved_stdout);
      }
      else if(rd==3)
      {
        dup2(saved_stdin,0);

        close(saved_stdin);
      }


    }
