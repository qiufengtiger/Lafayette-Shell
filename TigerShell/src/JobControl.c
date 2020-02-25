#include "JobControl.h"
#include <stdio.h>

int nextJid = 1;
jobData jobList[50];



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


