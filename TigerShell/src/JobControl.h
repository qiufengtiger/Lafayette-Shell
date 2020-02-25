#ifndef __JOBCTRL_H__
#define __JOBCTRL_H__

#include <time.h>


typedef struct jobData_struct
{
	int jid;
	int pid;
	int state;
	time_t startTime;
	int minFault;
	int maxFault;
	char commandName[20];
}jobData;


extern jobData jobList[50];
extern int nextJid;

int addJob(int jidInput, int pidInput, char* commandInput);
int deleteJob(int idInput);
int jobs();
int printJob(jobData jobDataInput);
int findAvailable();



#endif