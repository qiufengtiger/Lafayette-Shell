#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	printf("test on\n");
	printf("argc: %d\n", argc);
	printf("argv0: %s\n", argv[0]);
	printf("argv1: %s\n", argv[1]);
    if (argc != 2) { 
      fprintf(stderr, "usage: %s <string>\n", argv[0]);
      exit(1); 
    }
    while (1) {
      fprintf(stdout,"I want %s!\n", argv[1]);
      sleep(1);
      fflush(stdout);
    }

    return 0;
}


