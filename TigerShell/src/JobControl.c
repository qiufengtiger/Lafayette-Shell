#include "JobControl.h"
#include <stdio.h>

int nextJid = 1;
jobData jobList[50];


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
	printf("jid: %d\n", jobList[pos].jid);
	printf("pos: %d\n", pos);
	return 1;
}

int printJob(jobData jobDataInput){ 
	printf("%5d",jobDataInput.jid);
	printf("%5d",jobDataInput.pid);
	printf("%7d",jobDataInput.state);
	printf("%7s/n",jobDataInput.commandName);
	return 0;

}

int jobs(){
	printf("%5s","JID");
	printf("%5s","PID");
	printf("%7s","status");
	printf("%7s/n","command");
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
	printf("%5d",jobDataInput.pid);
	printf("%5d",jobDataInput.state);
	printf("%10f",(double)(clock()-jobDataInput.startTime));
	printf("%10d",jobDataInput.minFault);
	printf("%10d",jobDataInput.maxFault);
	printf("%7s/n",jobDataInput.commandName);
	return 0;

}

int jsum(){
	printf("%5s","PID");
	printf("%5s","status"); 
	printf("%10s","ElapsedTime");
	printf("%10s","Min Fault");
	printf("%10s","Max Fault");
	printf("%7s/n","commandName");
	for (int i = 0; i < 50; ++i){
		if(jobList[i].jid !=0){
			printJsum(jobList[i]);
		}
    		
  	} 
	return 0;

}


