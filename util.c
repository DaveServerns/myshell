#include "myshell.h"

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
