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
#include <sys/types.h>
#include <sys/msg.h>

#define MAXTIMEBETWEENNEWPROCSNS 1000
#define MAXTIMEBETWEENNEWPROCSSECS 1
#define MAX_PROC 10

struct processCtrlBlock
{
    float totalCpuTime;
    float timeInSystem;
    float lastBurstTime;
    int processPriority;
    bool inUse;
    int pid;
};

struct msgbuf
{
    long mtype;
    char mtext[50];
};

void cleanup();
struct processCtrlBlock createUserProcess(int);