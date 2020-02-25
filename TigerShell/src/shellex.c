/* $begin shellmain */
#include "csapp.h"
#include "JobControl.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 
char* getEnvVariable(char *inputArgv);
int assignJid();

char *prompt = "lsh";
int thisJid = -1; // current process pid
pid_t thisPid = -1; // current process jid
volatile int shutdownFlag;

// add a - sign in front of the pid to kill the entire group
void SIGINT_handler(int sig){
    if(thisPid == -1)
        exit(0);
    printf("Job %%%d / pid %d terminated by signal\n", thisJid, thisPid);
    shutdownFlag= 1;
}

void SIGTSTP_handler(int sig){
    if(thisPid == -1)
        exit(0);
    printf("Job %%%d / pid %d stopped by signal\n", thisJid, thisPid);
    shutdownFlag = 2;
}

int main() 
{
    shutdownFlag = 0;
    putenv("lshprompt=lsh");
    char cmdline[MAXLINE]; /* Command line */

    signal(SIGINT, SIGINT_handler); // CTRL-C
    signal(SIGTSTP, SIGTSTP_handler); // CTRL-Z
    printf("%d\n", findAvailable());
    while (1) {
	   /* Read */
	   printf("%s> ",getenv("lshprompt"));                   
	   Fgets(cmdline, MAXLINE, stdin); 
	   if (feof(stdin))
	       exit(0);
	   /* Evaluate */
       // printf("print cmdline: %s\n", cmdline);
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
    int pid; // child process pid
    int jid; // child process jid

    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    if(shutdownFlag == 1){
        Kill(0 - (int)thisPid, SIGINT);
    }
    else if(shutdownFlag == 2){
        Kill(0 - (int)thisPid, SIGTSTP);
    }

    if (!builtin_command(argv)) { 
        // child job
        jid = assignJid();
        printf("Job id: %d created!\n", jid);
        if ((pid = Fork()) == 0) {    
            thisPid = getpid();
            // printf("%d\n", getpgid(thisPid));
            printf("Process id: %d created!\n", thisPid);   
            
            // printf("%d\n", jobList[3].jid);
            if(argv[1] != NULL && *argv[1] == '$'){
                argv[1] = getEnvVariable(argv[1]);
                // printf("%s\n", argv[1]);
            }
            if (execvp(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
        // parent job
        else{
            addJob(jid, pid, argv[0]);
            testPrint(jid - 1);
            thisPid = getpid();
        }

	/* Parent waits for foreground job to terminate */
	if (!bg) {
	    int status;
	    if (waitpid(pid, &status, 0) < 0)
		  unix_error("waitfg: waitpid error");
        else 
            deleteJob(pid);
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
    
    /* set env variable */
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
    /* jobs */
    else if(!strcmp(argv[0], "jobs")){
        jobs();
        return 1;
    }
    else if(!strcmp(argv[0], "jsum")){
        jsum();
        return 1;
    }
    /* quit */
    else if(!strcmp(argv[0], "quit"))
	   exit(0);  
    else if(!strcmp(argv[0], "&"))
	   return 1;
    /* not a built in command */
    return 0;
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

int assignJid(){
    return nextJid++;
}
/* $end parseline */

