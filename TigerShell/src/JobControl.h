#ifndef __JOBCTRL_H__
#define __JOBCTRL_H__

#define JOB_LIST_SIZE 50
#define ALL_JOB_SIZE 50
#define JOB_COMMAND_SIZE 20
#define RUNNING 1
#define STOPPED 2
#define EXIT 3

#include <time.h>
#include <string.h>
#include <stdio.h>


typedef struct jobData_struct
{
	int jid;
	int pid;
	int state;
	time_t startTime;
	int minFault;
	int maxFault;
	char commandName[JOB_COMMAND_SIZE];
}jobData;


extern jobData jobList[JOB_LIST_SIZE];
extern int nextJid;

int addJob(int jidInput, int pidInput, char* commandInput);
int deleteJob(int idInput);
int continueJob(int idInput);
int jobs();
int printJob(jobData jobDataInput);
int findAvailable();

int testPrint(int input);

int printJsum(jobData jobDataInput);
int jsum();

int jobStopped(int idInput);
int jobExit(int idInput);



#endif

