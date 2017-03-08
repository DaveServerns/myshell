
#include "myshell.h"




int main(int argc, char  **argv) {

  clear_shell();
  // check to see if a "batchfile" is present
  // if there is we need to process the input from the batch file

  if((argv[1] != NULL)&&(strcmp(argv[1],"batchfile")==0))
  {
    char lineB[300];
		size_t length = 300;
		FILE *fp = fopen("batchfile", "r"); //Opens batchfile to read.

		if(fp == NULL){
			printf("Error. The file could not be opened. \n");
		}
    int bgB;
    while(fgets(lineB, length, fp)){ //Reads through each line.
			print_cmd_prompt();
			printf("%s", lineB);
      bgB=is_bg(lineB);
      char** batchArgs = get_cmd_args(lineB);
      eval(bgB,batchArgs);
      free(batchArgs);

    }


  }
  else //get input from the user
  {
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
    //fflush(stdout);
    }
  }
}
