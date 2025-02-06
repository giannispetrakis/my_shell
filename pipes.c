#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
# include <signal.h>

#include "shell.h"



int pipes(char * str){
	int i=0;
	while(str[i] != '\n' && str[i] != '\0'){
		if( str[i] == '|' )
			return 1;
		i++;
	}
	return 0;
	
}


int exec_pipes(char **argv, int in_fd, int out_fd) {
	
	int status;
	
	int background = check_background(argv);

	pid_t pid = fork();
	if (pid == -1) {
    	perror("fork");
    	return EXIT_FAILURE;
  	}
  	else if (pid == 0) { // child process
    	if (in_fd != STDIN_FILENO) { // input redirection
      		dup2(in_fd, STDIN_FILENO);
      		close(in_fd);
    	}
    	if (out_fd != STDOUT_FILENO) { // output redirection
      		dup2(out_fd, STDOUT_FILENO);
    	  	close(out_fd);
   		}
   		
   		//signal handling
    	static struct sigaction act ;
    	act . sa_handler = SIG_DFL ; // the handler is set to DEFAULT
		sigaction ( SIGINT , & act , NULL ) ; // default for control - c
		sigaction ( SIGTSTP , & act , NULL ) ; // default for control - z
    	
		execvp(argv[0], argv);
    	perror(argv[0]);
    	exit(EXIT_FAILURE);
  	}
  	else { // parent process
		if(background == 0){
			//run in foreground, wait for child to end/stop/interrupt
			waitpid(pid, &status, WUNTRACED);
		}
	
    	return EXIT_SUCCESS;
  	}
}


int handle_pipes(char *str) {
	char *pipes[1024];
	int num_pipes = 0;
	char *token = strtok(str, "|");
	while (token != NULL) {
		pipes[num_pipes++] = token;
    	token = strtok(NULL, "|");
  	}

	int i, in_fd = STDIN_FILENO, out_fd = STDOUT_FILENO;
	char *args[1024];
	for (i = 0; i < num_pipes; i++) {
    	int fd[2];
    	if (pipe(fd) == -1) {
      		perror("pipe");
      		return EXIT_FAILURE;
		}

    	// tokenize command string into argument array
    	int argc = 0;
    	token = strtok(pipes[i], " ");
    	while (token != NULL) {
    		args[argc++] = token;
    		token = strtok(NULL, " ");
    	}
    	args[argc] = NULL;

    	// execute command with input/output redirection
    	if (i == 0) { // first command
    		int n=0;
    		char *newargv[MAXCHARS];
    		char *file_input;
    	
    		//Check if pipe has also redirection (it can only have input redirection)
    		while(args[n] != NULL ){
    			newargv[n] = args[n];
    			if( !strcmp(args[n],"<") ){
    				//handle redirection
					newargv[n] = NULL;
					newargv[n+1] = NULL;
					
					file_input = args[n+1];
					in_fd = open(file_input, O_RDONLY);
					
      				if (in_fd == -1) {
        				perror(file_input);
        				return EXIT_FAILURE;
      				}
				}
				n++;
			}
			newargv[n] = NULL;
    		exec_pipes(newargv, in_fd, fd[1]);
    	
    	}
		else if (i == num_pipes - 1) { // last command
	
			int n=0;
    		char *newargv[MAXCHARS];
    		char *file_output;
    	
    		//Check if pipe has also redirection (it can only have output redirection)
    		while(args[n] != NULL ){
    			newargv[n] = args[n];
    			if( !strcmp(args[n],">") ){
    				//handle > redirection
					newargv[n] = NULL;
					newargv[n+1] = NULL;
					file_output = args[n+1];
					out_fd = open(file_output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      				if (out_fd == -1) {
        				perror(file_output);
        				return EXIT_FAILURE;
      				}
				}
			
				else if( !strcmp(args[n],">>") ){
					//handle >> redirection
					newargv[n] = NULL;
					newargv[n+1] = NULL;
					file_output = args[n+1];
			
					out_fd = open(file_output, O_WRONLY | O_APPEND, 0644);
      				if (out_fd == -1) {
        				perror(file_output);
        				return EXIT_FAILURE;
      				}
				}
				n++;
			}
			newargv[n] = NULL;
    		exec_pipes(newargv, in_fd, out_fd);
    	}
    	else { // middle command
    		exec_pipes(args, in_fd, fd[1]);
    	}

    	close(fd[1]);
    	//next input is the fd[0] of pipe
    	in_fd = fd[0];
	}

	return EXIT_SUCCESS;
}



