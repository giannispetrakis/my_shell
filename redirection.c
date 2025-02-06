#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
# include <signal.h>

#include "shell.h"


int redirection(char * str){
	int i=0;
	while(str[i] != '\n' && str[i] != '\0'){
		if( str[i] == '>' || str[i] == '<')
			return 1;
		i++;
	}
	return 0;
	
}



int handle_redirection(char *str){
	char **line = split(str);
	char *newargv[MAXCHARS];
	char *file_output, *file_input;
	int out, in, pid, status;
	
	//save default I/O 
	int saved_out = dup(STDOUT_FILENO);
	int saved_in = dup(STDIN_FILENO);
	
	
	int n = 0;
	while( line[n]!=NULL  ){
		newargv[n] = line[n];
		
		if( !strcmp(line[n],">") ){
			newargv[n] = NULL;
			newargv[n+1] = NULL;
			file_output = line[n+1];
			
			//open and get file descriptor
			out = open(file_output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      		if (out == -1) {
        		perror(file_output);
        		return EXIT_FAILURE;
      		}
      		//redirect output
      		dup2(out, STDOUT_FILENO);
      		close(out);
			break;
		}
		
		else if( !strcmp(line[n],"<") ){
			newargv[n] = NULL;
			newargv[n+1] = NULL;
			file_input = line[n+1];
			
			//open and get file descriptor
			in = open(file_input, O_RDONLY);
			if (in == -1) {
        		perror(file_input);
        		return EXIT_FAILURE;
      		}
      		//redirect input
      		dup2(in, STDIN_FILENO);
      		close(in);
      		
      		
      		n = n+2;
      		
      		//if command has ended break loop
      		if(line[n]==NULL){
      			break;
			}
      		//look for possible output
      		if( !strcmp(line[n],">") ){	
      			file_output = line[n+1];
				
				//open and get file descriptor
				out = open(file_output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      			if (out == -1) {
        			perror(file_output);
        			return EXIT_FAILURE;
      			}
      			n += 2;
      			
      			//redirect output
      			dup2(out, STDOUT_FILENO);
      			close(out);
      			break;
			}
			
			else{
				if( !strcmp(line[n],">>") ){
				
					file_output = line[n+1];
					//open and get file descriptor
					out = open(file_output, O_WRONLY | O_APPEND, 0644);
      				if (out == -1) {
        				perror(file_output);
        				return EXIT_FAILURE;
      				}
      				n+=2;
      				//redireect output
      				dup2(out, STDOUT_FILENO);
      				close(out);
      				break;
				}
			}
				
			  	
		}
		
		if( !strcmp(line[n],">>") ){
			newargv[n] = NULL;
			newargv[n+1] = NULL;
			file_output = line[n+1];
			
			//open and get file descriptor
			out = open(file_output, O_WRONLY | O_APPEND, 0644);
      		if (out == -1) {
        		perror(file_output);
        		return EXIT_FAILURE;
      		}
      		//redirect output
      		dup2(out, STDOUT_FILENO);
      		close(out);
      		break;
		}
		
		
		n++;
	}
	
	
	int background = check_background(newargv);
	
	if ( (pid = fork()) == 0) {
		
		static struct sigaction act ;
    	act . sa_handler = SIG_DFL ; // the handler is set to DEFAULT
		sigaction ( SIGINT , & act , NULL ) ; // default for control - c
		sigaction ( SIGTSTP , & act , NULL ) ; // default for control - z
		
      	(void) execvp(newargv[0],newargv);
		perror("error");
      	exit(1);
	}
	else {
		if(background == 0){
			//run in foreground, wait for child to end/stop/interrupt
			waitpid(pid, &status, WUNTRACED);
		}
			
	}
  	
	//reestablish deafult I/O
	dup2(saved_out, STDOUT_FILENO);
	close(saved_out);
	
	dup2(saved_in, STDIN_FILENO);
	close(saved_in);
	
	
	
}
