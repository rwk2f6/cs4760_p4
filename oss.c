#include "config.h"

//Globals
int * pid_list = NULL;
int * q1 = NULL;
int * q2 = NULL;
int * q3 = NULL;
int * q4 = NULL;
int * blockedQ = NULL;
int processCtrlTable_id, secTimer_id, nsecTimer_id, msgQ_id, curNumOfProcs = 0, totalNumOfProcs = 0;
struct processCtrlBlock * processCtrlTable_ptr = NULL;
unsigned int * secTimer_ptr = NULL;
unsigned int * nsecTimer_ptr = NULL;

struct processCtrlBlock createUserProcess(int procid)
{
    struct processCtrlBlock newProcess = { 0.0, 0.0, 0.0, 1, false, procid };
    return newProcess;
}

void cleanup()
{
    shmdt(processCtrlTable_ptr);
    shmdt(secTimer_ptr);
    shmdt(nsecTimer_ptr);

    shmctl(processCtrlTable_id, IPC_RMID, NULL);
    shmctl(secTimer_id, IPC_RMID, NULL);
    shmctl(nsecTimer_id, IPC_RMID, NULL);

    msgctl(msgQ_id, IPC_RMID, NULL);

    exit(0);
}

void cc_handler()
{
    printf("Received CTRL-C, terminating...\n");

    for (int i = 0; i < MAX_PROC; i++)
    {
        kill(pid_list[i], SIGTERM);
    }

    while(wait(NULL) > 0);

    cleanup();
}

int main(int argc, char *argv[])
{
    //Signal for Ctrl-c
    signal(SIGINT, cc_handler);

    //Allocate space for queues
    pid_list = malloc(sizeof(int) * MAX_PROC);
    q1 = malloc(sizeof(int) * MAX_PROC);
    q2 = malloc(sizeof(int) * MAX_PROC);
    q3 = malloc(sizeof(int) * MAX_PROC);
    q4 = malloc(sizeof(int) * MAX_PROC);
    blockedQ = malloc(sizeof(int) * MAX_PROC);

    printf("oss.c: Starting...\n");

    //Allocate shared memory for process control table
    key_t processCtrlTable_key = ftok(".", 'a');
    processCtrlTable_id = shmget(processCtrlTable_key, sizeof(struct processCtrlBlock) * MAX_PROC, IPC_CREAT | 0666);

    //Allocate shared memory for two integers that will keep track of the simulated system clock
    key_t secTimer_key = ftok("Makefile", 'a');
    secTimer_id = shmget(secTimer_key, sizeof(unsigned int), IPC_CREAT | 0666);

    key_t nsecTimer_key = ftok("uprocess.c", 'a');
    nsecTimer_id = shmget(nsecTimer_key, sizeof(unsigned int), IPC_CREAT | 0666);

    key_t msgQ_key = ftok("config.h", 'B');
    msgQ_id = msgget(msgQ_key, 0644 | IPC_CREAT);


    if (processCtrlTable_id == -1)
    {
        perror("oss.c: Error: Shared memory (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }
    if (secTimer_id == -1)
    {
        perror("oss.c: Error: Shared memory (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }
    if (nsecTimer_id == -1)
    {
        perror("oss.c: Error: Shared memory (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }
    if (msgQ_id == -1)
    {
        perror("oss.c: Error: Message queue (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }

    processCtrlTable_ptr = (struct processCtrlBlock *)shmat(processCtrlTable_id, 0, 0);
    secTimer_ptr = (unsigned int *)shmat(secTimer_id, 0, 0);
    nsecTimer_ptr = (unsigned int *)shmat(nsecTimer_id, 0, 0);

    //Initialize array of PCBs
    for (int i = 0; i < MAX_PROC; i++)
    {
        processCtrlTable_ptr[i] = createUserProcess(i);
    }

    printf("oss.c: Forking...\n");

    //Create user processes at random intervals, no more than 20 processes
    //Generate a new process by allocating and initializing the process control block, then fork and exec
    int j = 1;
    while (totalNumOfProcs < 3)
    {
        while (curNumOfProcs < 3)
        {
            struct msgbuf msgBuffer;

            sleep(1);

            processCtrlTable_ptr[j].pid = fork();

            if (processCtrlTable_ptr[j].pid == -1)
            {
                perror("Fork failed, program exiting early...\n");
                //Call some fork error handling
                cleanup();
            }

            if (processCtrlTable_ptr[j].pid == 0)
            {
                char * pnum = malloc(sizeof(char) * 5);
                sprintf(pnum, "%d", j);
                //Execl the child, though I'm not sure what parameters to pass with it
                execl("./uprocess", "./uprocess", pnum, NULL);
            }
            else
            {
                pid_list[j] = processCtrlTable_ptr[j].pid;
                j++;
                curNumOfProcs++;
                totalNumOfProcs++;
            }

            //Set msgBuffer
            msgBuffer.mtype = j;
            strcpy(msgBuffer.mtext, "You got mail!\0");
            int len = strlen(msgBuffer.mtext);

            if (msgsnd(msgQ_id, &msgBuffer, len+1, msgBuffer.mtype) == -1)
            {
                perror("oss.c: Error sending message, exiting...\n");
                cleanup();
            }
        }
    }

    printf("oss.c: Waiting for children...\n");
    while(wait(NULL) > 0);

    //oss schedules processes by sending messages
    printf("oss.c: Ending...\n");

    //Wait for a message that says the user process finished
    cleanup();
}
