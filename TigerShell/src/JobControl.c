#include "JobControl.h"

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

int jobs(){
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