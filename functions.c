#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
# include <signal.h>

#include "shell.h"


int input(char *str){
	char *buffer= malloc(sizeof(char) * MAXCHARS);
	
	for(int i = 0; i < MAXCHARS; i++){
		buffer[i] = '\0';
	}
	
	
	char c;
	int position=0;
	
	c = getchar();
	
	if(c == '\n' || c == EOF){
		return -1;
	}
	
	while(1){
		buffer[position] = c;
		position++;
		strcpy(str,buffer);
		c = getchar();
		if( c== '\n' || c == EOF){
			free(buffer);
			return position;
		}
		else if( c == ';'){
			buffer[position-1] = '\n';
			free(buffer);
			return position;
		}
		else if( c == '<'){
			buffer[position++] = ' ';
			buffer[position++] = c;
			c = ' ';
		}
		else if( c == '>'){
			buffer[position++] = ' ';
			buffer[position++] = c;
			c = getchar();
			if( c== '\n' || c == EOF){
				free(buffer);
				return position;
			}
			else if( c == ';'){
				buffer[position-1] = '\n';
				free(buffer);
				return position;
			}
			else if( c == '>'){
				buffer[position++] = c;
				c = ' ';
			}
			else{
				buffer[position++] = ' ';
			}
		}	
	}
	
}


char **split(char *line){
	
	char **args = malloc(MAXCHARS * sizeof(char*));
	
    char* token = strtok(line, " \n");

	int i = 0;
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;
    
    return args;
	
}

int check_background(char ** buffer){
	int n = 0;
	while(buffer[n] != NULL){
		if(!strcmp(buffer[n], "&") ){
			buffer[n] = NULL;
			return 1;
		}
		n++;
	}
	return 0;
}


int printHistory(char history[20][MAXCHARS]){
	if(!strcmp(history[0], "")){
		return -1;
	}
	printf("History(most recent first):\n");
	for(int i = 0; i < 20; i++){
		if( strcmp(history[i], "") )
			printf("%d: %s\n", i+1, history[i]);
	}
	return 0;
}

int simple_case_forking(char *str){	
	int status;
	int pid;
	static struct sigaction act ;
	
	char **buffer = split(str);
	int background = check_background(buffer);	
	
	if ( (pid = fork() ) == 0) {
				
		act . sa_handler = SIG_DFL ; // the handler is set to DEFAULT
		sigaction ( SIGINT , & act , NULL ) ; // default for control - c
		sigaction ( SIGTSTP , & act , NULL ) ; // default for control - z
				
      	(void) execvp(buffer[0],buffer);
		perror(buffer[0]);
      	exit(EXIT_FAILURE);
	}
	else if(pid < 0){
		perror("Fork failed ");
		return EXIT_FAILURE;
	}
	else{
		if(background == 0){
			//run in foreground, wait for child to end/stop/interrupt
			waitpid(pid, &status, WUNTRACED);
		}
		
		return EXIT_SUCCESS;
		
							
	}	
}
