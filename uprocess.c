#include "config.h"

//Globals
int processCtrlTable_id, secTimer_id, nsecTimer_id, msgQ_id;
struct processCtrlBlock * processCtrlTable_ptr = NULL;
unsigned int * secTimer_ptr = NULL;
unsigned int * nsecTimer_ptr = NULL;

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

int main(int argc, char *argv[])
{
    printf("Child: Started running...\n");

    struct msgbuf msgBuffer;

    //Get proc_id from execl
    int proc_id = atoi(argv[1]);

    //Attach to shared memory for process control table
    key_t processCtrlTable_key = ftok(".", 'a');
    processCtrlTable_id = shmget(processCtrlTable_key, sizeof(struct processCtrlBlock) * MAX_PROC, IPC_EXCL | 0666);

    //Attach to shared memory for two integers that will keep track of the simulated system clock
    key_t secTimer_key = ftok("Makefile", 'a');
    secTimer_id = shmget(secTimer_key, sizeof(unsigned int), IPC_EXCL | 0666);

    key_t nsecTimer_key = ftok("uprocess.c", 'a');
    nsecTimer_id = shmget(nsecTimer_key, sizeof(unsigned int), IPC_EXCL | 0666);

    //Attach to the message queue
    key_t msgQ_key = ftok("config.h", 'B');
    msgQ_id = msgget(msgQ_key, 0644 | IPC_EXCL);

    printf("Attached to shared memory and message queue...\n");

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

    sleep(5);

    printf("Child %d: Ready to receive messages...\n", proc_id);
    //printf("Child: Ready to receive messages...\n");

    for ( ; ; )
    {
        printf("Child: Inside for loop...\n");
        if (msgrcv(msgQ_id, &msgBuffer, sizeof(msgBuffer.mtext), msgBuffer.mtype, 0) == -1)
        {
            perror("Child: Error with msgrcv");
            exit(0);
        }
        char temp[50];
        strcpy(temp, msgBuffer.mtext);
        printf("Child: Message received: \"%s\"\n", temp);
        break;
    }

    printf("Child %d: Done receiving messages...\n", proc_id);

    shmdt(processCtrlTable_ptr);
    shmdt(secTimer_ptr);
    shmdt(nsecTimer_ptr);

    shmctl(processCtrlTable_id, IPC_RMID, NULL);
    shmctl(secTimer_id, IPC_RMID, NULL);
    shmctl(nsecTimer_id, IPC_RMID, NULL);

    msgctl(msgQ_id, IPC_RMID, NULL);

    return 0;
}