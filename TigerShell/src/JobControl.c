#include "JobControl.h"
#include <stdio.h>

int nextJid = 1;
jobData jobList[JOB_LIST_SIZE];
jobData allJobs[ALL_JOB_SIZE];


int addJob(int jidInput, int pidInput, char* commandInput){
	int pos = findAvailable();
	jobData thisJobData;
	thisJobData.jid = jidInput;
	thisJobData.pid = pidInput;
	thisJobData.state = RUNNING;
	thisJobData.startTime = time(0);
	thisJobData.minFault = 0;
	thisJobData.maxFault = 0;
	strcpy(thisJobData.commandName, commandInput);
	jobList[pos] = thisJobData;
	allJobs[jidInput] = thisJobData;
	
	// printf("jid: %d\n", jobList[pos].jid);
	// printf("pos: %d\n", pos);
	return 1;
}

int deleteJob(int idInput){
	// check both jid and pid
	int i = 0;
	for(i = 0; i < JOB_LIST_SIZE; i++){
		if(jobList[i].jid == idInput || jobList[i].pid == idInput){
			if(jobList[i].state == EXIT){

				printf("deleted: %d\n", jobList[i].pid);
				jobList[i].jid = 0; // jid = 0 will be seen as empty
			}
			return 1;
		}
	}
	return 0;
}

int continueJob(int idInput){
	int pid = 0;
	for(int i = 0; i < JOB_LIST_SIZE; i++){
		if(jobList[i].jid == idInput || jobList[i].pid == idInput){
			jobList[i].state = RUNNING;
			pid = jobList[i].pid;
		}
	}

	for(int i = 0; i < ALL_JOB_SIZE; i++){
		if(allJobs[i].jid == idInput || allJobs[i].pid == idInput){
			allJobs[i].state = RUNNING;
			break;
		}
	}
	return pid;
}

int printJob(jobData jobDataInput){ 
	printf("%10d",jobDataInput.jid);
	printf("%10d",jobDataInput.pid);
	if (jobDataInput.state == RUNNING){
		printf("%15s","RUNNING");
	}
	else if(jobDataInput.state == STOPPED){
		printf("%15s","STOPPED");
	}
	else{
		printf("%15s","EXIT");
	}
	printf("%15s\n",jobDataInput.commandName);
	return 0;

}

int jobs(){
	printf("%10s","JID");
	printf("%10s","PID");
	printf("%15s","status");
	printf("%15s\n","command");
	for (int i = 0; i < 50; ++i){
		if(jobList[i].jid !=0){
			printJob(jobList[i]);
		}
    		
  	} 
	return 0;
}

int findAvailable(){
	int i = 0;
	for(i = 0; i < JOB_LIST_SIZE; i++){
		if(jobList[i].jid == 0)
			return i;
	}
	return -1;
}

int testPrint(int input){
	printf("%s\n", jobList[input].commandName);
	return 0;
}
int printJsum(jobData jobDataInput){
	int seconds = (int)(time(0)-jobDataInput.startTime);
	int hours = seconds/(60*60);
	int remained = seconds-hours*60*60;
	int minutes = remained/60;

	remained = remained-minutes*60;
	printf("%10d",jobDataInput.pid);
	if (jobDataInput.state == RUNNING || jobDataInput.state == EXIT){
		printf("%10s","ok");
	}
	else if(jobDataInput.state == STOPPED){
		printf("%10s","STOPPED");
	}
	else{
		printf("%10s","ERROR");
	}


	if(hours<10){
		printf("%14d:",hours);
	}
	else{
		printf("%14d:",hours);
	}
	if(minutes<10){
		printf("0%d:",minutes);
	} 
	else{
		printf("%d:",minutes);
	}
	if(remained<10){
		printf("0%d",remained);
	}
	else{
		printf("%d",remained);
	}
	printf("%13d",jobDataInput.minFault);
	printf("%13d",jobDataInput.maxFault);
	printf("%15s\n",jobDataInput.commandName);
	return 0;

}

int jsum(){
	printf("%10s","PID");
	printf("%10s","status"); 
	printf("%19s","ElapsedTime");
	printf("%13s","Min Fault");
	printf("%13s","Maj Fault");
	printf("%15s\n","commandName");
	for (int i = 0; i < 50; ++i){
		if(allJobs[i].jid !=0){
			printJsum(allJobs[i]);
		}
    		
  	} 
	return 0;

}

int jobStopped(int idInput){
	for(int i = 0; i < JOB_LIST_SIZE; i++){
		if(jobList[i].jid == idInput || jobList[i].pid == idInput){
			jobList[i].state = STOPPED;
		}
	}

	for(int i = 0; i < ALL_JOB_SIZE; i++){
		if(allJobs[i].jid == idInput || allJobs[i].pid == idInput){
			allJobs[i].state = STOPPED;
			return 1;
		}
	}
	return 0;
}

int jobExit(int idInput){
	for(int i = 0; i < JOB_LIST_SIZE; i++){
		if(jobList[i].jid == idInput || jobList[i].pid == idInput){
			jobList[i].state = EXIT;
		}
	}

	for(int i = 0; i < ALL_JOB_SIZE; i++){
		if(allJobs[i].jid == idInput || allJobs[i].pid == idInput){
			printf("exit stated written: %d\n", idInput);
			allJobs[i].state = EXIT;
			return 1;
		}
	}
	return 0;
}

int setPageFault(int jid, long int minPF, long int majPF){
	allJobs[jid].minFault = minPF;
	allJobs[jid].maxFault = majPF;
	return 0;


}


