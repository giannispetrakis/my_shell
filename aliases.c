#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
# include <signal.h>

#include "shell.h"




int createalias(char * str){
	int i=0;
	//find the first 'non empty' character
	while(str[i] == ' '){
		i++;
	}
	char *str2 = "createalias";
    for (int j = 0; j < strlen(str2); j++) {
        if (str[i] != str2[j]) {
            return 0;
        }
        i++;
    }
    if (str[i] == ' ' || str[i] == '\0') {  
        return 1;
    }
    return 0;

}


int destroyalias(char * str){
	int i=0;
	//find the first 'non empty' character
	while(str[i] == ' '){
		i++;
	}
	char *str2 = "destroyalias";
    for (int j = 0; j < strlen(str2); j++) {
        if (str[i] != str2[j]) {
            return 0;
        }
        i++;
    }
    if (str[i] == ' ' || str[i] == '\0') {  
        return 1;
    }
    return 0;

}


int search_alias(char ** array, char * str, int size){
	
	//size 1 means empty arrays
	if(size == 1){
		return NOT_FOUND;
	}
	
	for(int i = 0; i < size; i++){
		
		if(!strcmp(array[i], str)){
			//if alias is found return position where it is
			return i;
		}
			
	}
	
	return NOT_FOUND;
	
}


char **add_one(char **old_array, int old_size, char * str){
    int new_size = old_size + 1; 
    // allocate memory for the new array
    char **new_array = (char **)realloc(old_array, sizeof(char *) * new_size);
    if (new_array == NULL) {
        return NULL;
    }
    
    char *new_str = (char *)malloc(sizeof(char) * MAXCHARS);
    if(new_str == NULL){
    	return NULL;
	}
	
    sprintf(new_str, "%s", str);
    // add the new string to the end of the array
    new_array[old_size-1] = new_str;
    new_array[new_size-1] = "\0";
    
    
    return new_array;
}


int handle_createalias(char *str, char **alias, char **real, int size){
	char ** tokened = split(str);
	
	if(tokened[1] == NULL){
		printf("Error: Too few arguments for createalias\n");
		return -1;
	}
	
	if(tokened[2] == NULL){
		printf("Error: Too few arguments for createalias\n");
		return -1;
	}
	
	//add to alias array
	add_one(alias, size, tokened[1]);
	int i = 2;	
	char temp[MAXCHARS];
	strcpy(temp, "");
	
	
	
			
	while(tokened[i] != NULL){
		//if first character is ", remove it
		if(tokened[i][0] == '"'){
			tokened[i][0] = ' ';
		}
		strcat(temp, tokened[i]);
		strcat(temp, " ");
		i++;
	}
	//if last character is ", remove it
	int j = 0;
	while(tokened[i-1][j] != '\0'){
		if(tokened[i-1][j] == '"'){
			int ssize = strlen(temp);
			temp[ssize-2] = ' ';
		}
		j++;
	}
				
	//after " removal, add to real array	
	add_one(real, size, temp);
	return 0;
}


int remove_one(char **array1, char **array2, char *str, int size) {
    int i = 0;
    int found = 0;
    while (i < size) {
        if (!strcmp(array1[i], str)) {
            found = 1;
            break;
        }
        i++;
    }
    if (!found) {
        return size;  // string not found, return original size
    }
    free(array1[i]);
    free(array2[i]);
    for (int j = i; j < size - 1; j++) {
        array1[j] = array1[j+1];
        array2[j] = array2[j+1];
    }
    size--;
    array1 = (char **)realloc(array1, sizeof(char *) * size);
    array2 = (char **)realloc(array2, sizeof(char *) * size);
    return size;
}


int handle_destroyalias(char *str, char **alias, char **real, int size){
	char ** tokened = split(str);
	int i = 0;
	while(tokened[i] != NULL){
		i++;
	}
	if(i == 1){
		printf("Error: too few arguments for destroyalias\n");
		return -1;
	}
	else if(i > 2){
		printf("Error: too many arguments for destroyalias\n");
		return -1;
	}
	else{
		remove_one(alias, real, tokened[1], size);				
	}
	
	return 0;
	
}


void free_alias_arrays(char **array1, char **array2, int size){
	if(size == 1){
		return;
	}
	
	int n = 0;
	while( strcmp(array1[n],"\0" ) ){
		free(array1[n]);
        free(array2[n]);
		n++;
	}

    free(array1);
    free(array2);
}



