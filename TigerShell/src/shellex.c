/* $begin shellmain */
#include "csapp.h"
#include "JobControl.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>

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
    // printf("pid: %d\n", pid);
    if(!bg && pid != getpid() && pid != 0){
        printf("pid in sig handler: %d, called in %d\n", pid, getpid());
        Kill(pid, sig);
        jobExit(pid);
        deleteJob(pid);
    }
}

void SIGTSTP_handler(int sig){
    // printf("bg: %d\n", bg);
    if(!bg && pid != getpid() && pid != 0){
        printf("pid in sig handler: %d\n", pid);
        Kill(pid, sig);
        jobStopped(pid);
    }
    
}

// void SIGCHLD_handler(int sig){
//     printf("SIGCHLD pid: %d\n", pid);
//     int thisStatus;
//     // waitpid(pid, &status, 0);
//     if(WIFEXITED(status) && !WIFSTOPPED(status)){
//         printf("exit detected! pid: %d\n", pid);
//         jobExit(pid);   
//     }
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
    // store this bg regardless of command type
    // overwrite bg only when not builtin job
    int thisBg = parseline(buf, argv);
    if (argv[0] == NULL)  
        return;   /* Ignore empty lines */    
    int pipepos = -1;
    // printf("0001\n");

    int i = 0;

    for(i = 0; ; i++){
        // printf("argv %d : %s\n", i, argv[i]);
        if(argv[i] == NULL){
            break;
        }
        if(*argv[i] == '|'){
            pipepos = i;
            break;
        }
        if(i > 8)
            break;
    }

    if(pipepos != -1){
        int fdpipes[2];
	    if(pipe(fdpipes) == -1){
	        printf("failed to create pipe");
		    exit(1);
	    }
	    char* firstCommand[4];
	    char* secondCommand[4];
        // printf("1111\n");
	    for(int i = 0; i < pipepos; i++){
		    firstCommand[i] = argv[i];
		    printf("%s\n",argv[i]);
	    }
        // printf("2222\n");
	    for(int i = pipepos + 1; i < 8; i++){
            if(argv[i] == NULL)
                break;
		    secondCommand[i - pipepos - 1] = argv[i];
            printf("%s\n",argv[i]);
	    }
	    
	    jid = assignJid();
	    pid_t pid_parent = fork();


	    if(pid_parent == 0){
	   	    dup2(fdpipes[1], STDOUT_FILENO);
		    close(fdpipes[0]); 
		    close(fdpipes[1]); 
	
	   	    execvp(firstCommand[0], firstCommand);
           	    return ;
             }
	     
             addJob(jid, pid, firstCommand[0]);

	
	    
            jid = assignJid();
	    pid_t pid_child = fork();


	    if(pid_child == 0){
	   	    dup2(fdpipes[0], STDIN_FILENO);
		    close(fdpipes[0]); 
		    close(fdpipes[1]); 
	   	    execvp(secondCommand[0],secondCommand);
           	return ;
            }
	    
            addJob(jid, pid, secondCommand[0]);

	    close(fdpipes[0]);
   	    close(fdpipes[1]); 
	    while (wait(NULL) > 0);
        return ;
    }
    if (!builtin_command(argv)) { 
        bg = thisBg;
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
                printf("job added %s\n", argv[0]);
                addJob(jid, pid, argv[0]);
        }

    	/* Parent waits for foreground job to terminate */
	
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
    	int beginMin = usage.ru_minflt;
    	int beginMaj = usage.ru_majflt; 


    	if (!bg) {
            
            if(waitpid(pid, &status, WUNTRACED) < 0)
                unix_error("waitfg: waitpid error");
            if(WIFEXITED(status)){
                printf("exit caught in eval: %d\n", pid);
                jobExit(pid);
            }
            deleteJob(pid);
            
        }
	getrusage(RUSAGE_SELF, &usage);
    	//printf("min%li\n",usage.ru_minflt-beginMin);
    	//printf("max%li\n",usage.ru_majflt-beginMaj);
	setPageFault(jid, usage.ru_minflt-beginMin, usage.ru_majflt-beginMaj);
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
        char *perSignPos = NULL;
        char newPidInput[(int)strlen(argv[1]) + 1];
        perSignPos = strchr(argv[1], '%');
        // read pid or jid
        if(perSignPos != NULL){
            strcpy(newPidInput, perSignPos + 1);
            pid = atoi(newPidInput);
        }
        else{
            pid = atoi(argv[1]);
        }      
        pid = continueJob(pid);
        // printf("%d\n", pid);
        setpgid(pid, getpgid(getpid()));
        bg = 0;
        Kill(pid, SIGCONT);
        waitpid(pid, &status, WUNTRACED);
        return 1;
    }
    else if(!strcmp(argv[0], "bg")){
        char *perSignPos = NULL;
        char newPidInput[(int)strlen(argv[1]) + 1];
        perSignPos = strchr(argv[1], '%');
        // read pid or jid
        if(perSignPos != NULL){
            strcpy(newPidInput, perSignPos + 1);
            pid = atoi(newPidInput);
        }
        else{
            pid = atoi(argv[1]);
        }      
        pid = continueJob(pid);
        setpgid(pid, 0);
        bg = 1;
        Kill(pid, SIGCONT);
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

