#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
# include <signal.h>
#include <glob.h>

#include "shell.h"



int wild_characters(char * str){
	int i=0;
	while(str[i] != '\n' && str[i] != '\0'){
		if( str[i] == '*' || str[i] == '?' )
			return 1;
		i++;
	}
	return 0;
}


char * handle_wildcharacters(char * str){
	
	char *new_str = malloc(MAXCHARS * sizeof(char));
	strcpy(new_str, "");
	char **tokened = split(str);
	
	int i = 0;
	while(tokened[i] != NULL){
		if( wild_characters(tokened[i]) ){
			//if word has wild_characters
			
			char *pattern = tokened[i];
    		glob_t results;

    		int ret = glob(pattern, 0, NULL, &results);
    		if (ret != 0) {
        		printf("No file matching %s\n", pattern);
        		return GLOBERROR;
    		}
			
			if(i == 0){
				//add first match to new_str
				strcat(new_str, results.gl_pathv[0]);
			}
			else{
				for (int j = 0; j < results.gl_pathc; j++) {
					//add all matches to new_str
        			strcat(new_str, " ");
					strcat(new_str, results.gl_pathv[j]);
    			}
			}

    		globfree(&results);
		}
		else{
			//if word does not have wild_characters add it to new_str
			strcat(new_str, " ");
			strcat(new_str, tokened[i]);
		}
		i++;
	}

	return new_str;
}




