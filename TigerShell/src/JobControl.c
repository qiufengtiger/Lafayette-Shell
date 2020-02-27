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
			jobList[i].jid = 0; // jid = 0 will be seen as empty
			return 1;
		}
	}
	return 0;
}

int printJob(jobData jobDataInput){ 
	printf("%10d",jobDataInput.jid);
	printf("%10d",jobDataInput.pid);
	printf("%15d",jobDataInput.state);
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
	printf("%10d",jobDataInput.state);
	printf("%15d:%d:%d",hours,minutes,remained);
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
	printf("%13s","Max Fault");
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
			return 1;
		}
	}

	for(int i = 0; i < ALL_JOB_SIZE; i++){
		if(allJobs[i].jid == idInput || jobList[i].pid == idInput){
			allJobs[i].state = STOPPED;
			return 1;
		}
	}
	return 0;
}
int jobExit(int idInput){
	for(int i = 0; i < ALL_JOB_SIZE; i++){
		if(allJobs[i].jid == idInput || jobList[i].pid == idInput){
			allJobs[i].state = EXIT;
			return 1;
		}
	}
	return 0;
}


