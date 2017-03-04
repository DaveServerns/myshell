#include <stdio.h>
#include <string.h>



/*#define READ 0
#define WRITE 1
#define BUF_SIZE 64


int my_pipe(int argc, char** inputs){
  pid_t pid;
  int fd[2];
  char buf[BUF_SIZE];
  int bytes_read;

  if (pipe(fd)==-1) {
    fprintf(stderr, "pipe error\n");
    return EXIT_FAILURE;
  }

  if ((pid=fork())==-1) {
    fprintf(stderr, "fork failure: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  else if (pid == 0) {
    close(fd[READ]);
    dup2(fd[WRITE], STDOUT_FILENO);
    if (execl("/bin/date", "/bin/date", NULL)<0) {
      fprintf(stderr, "exec error: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }
  }
  else {
    close(fd[WRITE]);
    dup2(fd[READ], STDIN_FILENO);
    if ((bytes_read=read(STDIN_FILENO, buf, BUF_SIZE-1))>=0) {
      buf[bytes_read]=’\0’;
      printf("I’m the parent and I just read %s\n", buf);
    }
  }
  return EXIT_SUCCESS;


}*/ //fix this

/*need to check for redirect symbols in cmd stream this will check for
">", ">>", and "<" symbols and returns a flag to determine if they are present
and if they are, which one is envoked

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
      continue;
  }
  return r_flag;

}*/
