/* $begin shellmain */
#include "csapp.h"
#include "JobControl.h"
#include <fcntl.h>
#include <sys/types.h>

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
int pid; // child process pid
int jid; // child process jid
int bg;
int execStatus;
int status;

void SIGINT_handler(int sig){
    if(!bg){
        printf("pid in sig handler: %d, called in %d\n", pid, getpid());
        Kill(-1 * pid, sig);
        jobExit(pid);
    }
}

void SIGTSTP_handler(int sig){
    if(!bg){
        printf("pid in sig handler: %d\n", pid);
        Kill(-1 * pid, sig);
        jobStopped(pid);
    }
    
}

// void SIGCHLD_handler(int sig){
//     // while(waitpid((pid_t)(-1), &status, WNOHANG) > 0){

//     // }
// }


int main() 
{   

    signal(SIGINT, SIGINT_handler); // CTRL-C
    signal(SIGTSTP, SIGTSTP_handler); // CTRL-Z  
    // signal(SIGCHLD, SIGCHLD_handler); // child termination listener 

    putenv("lshprompt=lsh");
    char cmdline[MAXLINE]; /* Command line */
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
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */    

    if (!builtin_command(argv)) { 
        jid = assignJid();
        if ((pid = Fork()) == 0) {
            if(argv[1] != NULL && *argv[1] == '$'){
                argv[1] = getEnvVariable(argv[1]);
            }

            else if(argv[1] != NULL && argv[2] != NULL && *argv[2] == '>'){ // redirect output to file
                int fd = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);               
                argv[2] = NULL;
                argv[3] = NULL;
            }
            if(bg){
                setpgid(getpid(), 0);
            }
            execStatus = execvp(argv[0], argv);
            if (execStatus < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
        // parent job
        else{
            if(execStatus >= 0)
                addJob(jid, pid, argv[0]);
        }

    	/* Parent waits for foreground job to terminate */
    	if (!bg) {
            
            if (waitpid(pid, &status, WUNTRACED) < 0)
                unix_error("waitfg: waitpid error");
            deleteJob(pid);
        }
        // printf("parent job wait skipped complete\n");
	    
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
    else if(!strcmp(argv[0], "fg")){
        pid = atoi(argv[1]);
        setpgid(pid, getpgid(getpid()));
        continueJob(pid);
        Kill(-1 * pid, SIGCONT);
        waitpid(pid, &status, WUNTRACED);
        return 1;
    }
    else if(!strcmp(argv[0], "bg")){
        bg = 1;
        pid_t targetPid = atoi(argv[1]);
        continueJob(pid);
        setpgid(targetPid, 0);
        Kill(-1 * pid, SIGCONT);
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

