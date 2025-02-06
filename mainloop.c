#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
# include <signal.h>

#include "shell.h"


void shellLoop(){
	
	static struct sigaction act ;
	act . sa_handler = SIG_IGN ; // the handler is set to IGNORE
	sigfillset (&( act . sa_mask ) ) ;

	//arrays for aliases
	char **real = (char **)malloc((sizeof(char) * MAXCHARS) * 1);
	char **alias = (char **)malloc((sizeof(char) * MAXCHARS) * 1);
	int size = 1; //alias array size, starting with 1
	
 	char history[20][MAXCHARS];
 	
    while(1){
    	
    	sigaction ( SIGINT , & act , NULL ) ; // ignore control - c
		sigaction ( SIGTSTP , & act , NULL ) ; // ignore control - z
    	
    	char str[MAXCHARS];
    	int pid, status;
    	
    	printf("in-mysh-now>");
    	
    	//if input is enter, continue
    	if( input(str) == -1 ){
    		continue;
		}
		
		if(!strcmp(str, "history")){
			if(printHistory(history) == EMPTY){
				printf("No history yet\n");
				continue;
			}
 			
 			printf("To run a past command, type the number\
 of the command you want, or press enter to exit history access.\n");
 			
 			char position_str[MAXCHARS];
			if(input(position_str) == -1){
				continue;
			}
			
			
			int position = atoi(position_str);
			if(position == 0 || strcmp(history[position-1], "") == 0 ){
				printf("Invalid input\n");
				continue;
			}
 			strcpy(str, history[position-1]);
		}

		//insert str to the first position in history, move the others down
		for(int j = 19; j > 0; j--){
			strcpy(history[j],history[j-1]);
		}
		strcpy(history[0],str);
		
		
		//if * or ? is included in str 
		if(wild_characters(str) ){
			char * str_new = handle_wildcharacters(str);
			if(str_new == GLOBERROR){	//nothing is found
				continue;
			}
			strcpy(str, str_new);
		}
		
		
		if(createalias(str)){
			if(handle_createalias(str, alias, real, size) != -1 ){
				size++;
			}
			continue;
		}
		else if(destroyalias(str)){
			if(handle_destroyalias(str, alias, real, size) != -1){
			    size--;
			}
			continue;
		}
		else{
			// search for possible aliases in str
			char str2[MAXCHARS];
			strcpy(str2,str);
			char ** tokened = split(str2);
			strcpy(str,"");
			
			int i = 0;
			//for each word
			while(tokened[i] != NULL){
				if(i){
					strcat(str, " ");
				}
				int search;
				
				
				if( (search = search_alias(alias, tokened[i], size)) == NOT_FOUND){
					//if word is not alias strcat it into the str
					strcat(str, tokened[i]);
				}
				else{
					//if word is alias strcat the real into the str
					strcat(str, real[search]);				
				}
				
				i++;	
			}
			
		}
	
		if(pipes(str)){
			handle_pipes(str);
			continue;
		}
		else if(redirection(str)){
			handle_redirection(str);
			continue;
		}
		else if(!strcmp(str,"exit")){
			break;
		}
		else{
			simple_case_forking(str);
			continue;
    	
		}
	}
	free_alias_arrays(alias, real, size);
}

int main(){
 	
 	shellLoop();
	
}
