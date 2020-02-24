/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 
char* getEnvVariable(char *inputArgv);

char *prompt = "lsh";

void SIGINT_handler(int sig){
    printf("got SIGINT");
    exit(0);
}

void SIGTSTP_handler(int sig){
    printf("got SIGTSTP");
}

int main() 
{
    putenv("lshprompt=lsh");
    char cmdline[MAXLINE]; /* Command line */
    signal(SIGINT, SIGINT_handler); // CTRL-C
    signal(SIGTSTP, SIGTSTP_handler); // CTRL-Z
    while (1) {
	   /* Read */
	   printf("%s> ",getenv("lshprompt"));                   
	   Fgets(cmdline, MAXLINE, stdin); 
	   if (feof(stdin))
	       exit(0);
	   /* Evaluate */
	   eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { 
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            // printf("%d\n", strlen(getEnvVariable("$PATH")));
            // char pathEnvp[5 + 1 + strlen(getEnvVariable("$PATH"))];
            // strcpy(pathEnvp, "PATH=");
            // strcat(pathEnvp, getEnvVariable("$PATH"));
            if (execvp(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

	/* Parent waits for foreground job to terminate */
	if (!bg) {
	    int status;
	    if (waitpid(pid, &status, 0) < 0)
		unix_error("waitfg: waitpid error");
	}
	else
	    printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    char *equalSignPos = NULL;
    
    equalSignPos = strchr(argv[0], '=');
    
    /*set env variable*/
    if(equalSignPos != NULL){
        char desName[equalSignPos - argv[0] + 1];
        char srcName[(int)strlen(argv[0]) - (int)(equalSignPos - argv[0])];
        strncpy(desName, argv[0], equalSignPos - argv[0]);
        desName[equalSignPos - argv[0]] = 0;
        strcpy(srcName, equalSignPos + 1);
        if(strlen(srcName) == 0){
            unsetenv(desName);
            return 1;
        }
        setenv(desName, srcName, 1);

        printf("%s\n", "success");
        return 1;
    }


    if (!strcmp(argv[0], "quit")) /* quit command */
	   exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
	   return 1;
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}

char* getEnvVariable(char *inputArgv){
    char *input = inputArgv;
    char *returnArray = NULL;
    char *dollarSignPos = NULL;
    dollarSignPos = strchr(input, '$');
    if(dollarSignPos !=NULL){
       char srcName[(int)strlen(input) - (int)(dollarSignPos - input)];
       strcpy(srcName, dollarSignPos + 1);
       returnArray = getenv(srcName);
    }
    return returnArray;
}
/* $end parseline */
