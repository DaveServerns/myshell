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
		execvp(readEnd[0],NULL);

	}

	else if ((cpid = fork()) == 0){				// Second child running cmd1 with new stdout
    close(fd[0]);
    dup2(fd[1], 1);							// Setting stdout to write end of pipe
		execvp(writeEnd[0],NULL);


	}

	else {
		dup2(1, fd[1]);
		fflush(stdout);
		waitpid(cpid, NULL, 0);
	}
}


int main(int argc, char const *argv[]) {
  char** read;
  char** writel;
  read[0]="ls";
  writel[0] = "wc";
  my_pipe(read,writel);
  return 0;
}
