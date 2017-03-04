#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/param.h>
#include "myshell.h"




int main(int argc, char  **argv) {

  clear_shell();
  int i,bg;
  i=0;
  while(1){
    //print out current path
    print_cmd_prompt();
    char *line = NULL;
    unsigned long len = MAXPATHLEN;
    //read user inpt from keyboard
    getline(&line,&len, stdin);
    //if input is "quit" exit program
    //else evaluate the commands
    if (strcmp(line,"quit\n")==0){
      exit(0);
    }
    bg = is_bg(line);

    char** args = get_cmd_args(line);
    eval(bg,args);

    free(line);
    free(args);
    flush(stdout);

  }

}
