#include "config.h"

//Globals
int * pid_list = NULL;
int processCtrlTable_id, secTimer_id, nsecTimer_id;
processCtrlBlock * processCtrlTable_ptr = NULL;
unsigned int * secTimer_ptr = NULL;
unsigned int * nsecTimer_ptr = NULL;

processCtrlBlock createUserProcess(int procid)
{
    processCtrlBlock newProcess;
    newProcess.pid = procid;
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

    exit(0);
}

int main(int argc, char *argv[])
{
    //Allocate shared memory for process table with a process control block for each process
    pid_list = malloc(sizeof(int) * MAX_PROC);
    key_t processCtrlTable_key = ftok(".", 'a');
    processCtrlTable_id = shmget(processCtrlTable_key, sizeof(processCtrlBlock) * MAX_PROC, IPC_CREAT | 0666);

    //Allocate shared memory for two integers that will keep track of the simulated system clock
    key_t secTimer_key = ftok("Makefile", 'a');
    secTimer_id = shmget(secTimer_key, sizeof(unsigned int), IPC_CREAT | 0666);

    key_t nsecTimer_key = ftok("uprocess.c", 'a');
    nsecTimer_id = shmget(nsecTimer_key, sizeof(unsigned int), IPC_CREAT | 0666);

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

    processCtrlTable_ptr = (processCtrlBlock *)shmat(processCtrlTable_id, 0, 0);
    secTimer_ptr = (unsigned int *)shmat(secTimer_id, 0, 0);
    nsecTimer_ptr = (unsigned int *)shmat(nsecTimer_id, 0, 0);

    //Create user processes at random intervals, no more than 20 processes
    //Generate a new process by allocating and initializing the process control block, then fork and exec
    for (int i = 0; i < MAX_PROC; i++)
    {
        userProcess = createUserProcess(fork());

        if (userProcess.pid == -1)
        {
            perror("Fork failed, program exiting early...\n");
            //Call some fork error handling
            cleanup();
        }

        if (userProcess.pid == 0)
        {
            //Execl the child, though I'm not sure what parameters to pass with it
            execl("./uprocess", "./uprocess", NULL);
        }
        else
        {
            pid_list[i] = userProcess.pid;
        }
    }

    //oss schedules processes by sending messages

    //Wait for a message that says the user process finished
}
