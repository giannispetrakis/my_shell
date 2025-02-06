#define MAXCHARS 2048
#define GLOBERROR "globerror_1"
#define EMPTY -1
#define NOT_FOUND -1


int redirection(char *);
int handle_redirection(char *);

int pipes(char *);
int exec_pipes(char **, int, int);
int handle_pipes(char *);

int wild_characters(char *);
char * handle_wildcharacters(char *);

int createalias(char *);
int destroyalias(char *);
int search_alias(char **, char *, int);
char **add_one(char **, int, char *);
int handle_createalias(char *, char **, char **, int);
int remove_one(char **, char **, char *, int);
int handle_destroyalias(char *, char **, char **, int);
void free_alias_arrays(char **, char **, int);

int input(char *);
char **split(char *);
int check_background(char **);
int printHistory(char [20][MAXCHARS]);
int simple_case_forking(char *);

