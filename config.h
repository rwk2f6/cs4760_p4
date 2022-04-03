#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/ipc.h>
#include <errno.h> 
#define MAXTIMEBETWEENNEWPROCSNS 1000
#define MAXTIMEBETWEENNEWPROCSSECS 1
#define MAX_PROC 1

struct processCtrlBlock
{
    float totalCpuTime = 0;
    float timeInSystem = 0;
    float lastBurstTime = 0;
    int processPriority = 1;
    bool inUse = false;
    int pid;
};

processCtrlBlock createUserProcess(int);
