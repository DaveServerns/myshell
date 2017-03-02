#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <dirent.h>


void print_cmd_prompt(){
  /*get the current path
    print the path (cwd)*/
    char buf[MAXPATHLEN];
    printf("%s=:> ",  getcwd(buf,sizeof(buf)));
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
    token = strtok(str, " ,\n");

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
      token = strtok(NULL, " ,\n");

    }
    //set end of tokens to NULL
    tokens[index]= NULL;
    return tokens;

}

/*will run when "clr" is entered, for now just prints \n char to clear the screen
  run loop 100 times clears screen even in maxim mode*/
void clear_shell(){
  int s;
  for(s=0;s<100;s++)
  {
    printf("\n");
  }
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


/* eval will read the args that have been tokenizes and perfrom the appropriate builin function
if no function exists it will run the command as an exec
*/
void eval(char** inputs){

    //determine amount of arguments passed to cmd line
    int argc=0;
    int i;
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
      printf("%s\n", "No such command...try again");

    //free(cmd_1);

}
