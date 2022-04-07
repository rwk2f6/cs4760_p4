#include "config.h"

//Globals
int processCtrlTable_id, secTimer_id, nsecTimer_id, msgQ_id;
struct processCtrlBlock * processCtrlTable_ptr = NULL;
unsigned int * secTimer_ptr = NULL;
unsigned int * nsecTimer_ptr = NULL;

void cc_handler()
{
    printf("Received CTRL-C, terminating...\n");

    kill(getpid(), SIGTERM);

    cleanup();
}

void cleanup()
{
    shmdt(processCtrlTable_ptr);
    shmdt(secTimer_ptr);
    shmdt(nsecTimer_ptr);

    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, cc_handler);

    struct msgbuf msgBuffer;

    //Get proc_id from execl
    long proc_id = atoi(argv[1]);

    printf("Child %ld: Started running...\n", proc_id);

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

    printf("Child %ld: Attached to shared memory and message queue...\n", proc_id);

    if (processCtrlTable_id == -1)
    {
        perror("uprocess.c: Error: Shared memory (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }
    if (secTimer_id == -1)
    {
        perror("uprocess.c: Error: Shared memory (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }
    if (nsecTimer_id == -1)
    {
        perror("uprocess.c: Error: Shared memory (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }
    if (msgQ_id == -1)
    {
        perror("uprocess.c: Error: Message queue (buffer) could not be created\n");
        printf("Error, exiting\n\n");
        cleanup();
    }

    //sleep(5);

    printf("Child %ld: Ready to receive messages...\n", proc_id);
    //printf("Child: Ready to receive messages...\n");

    for ( ; ; )
    {
        printf("Child %ld: Inside for loop...\n", proc_id);
        if (msgrcv(msgQ_id, &msgBuffer, sizeof(msgBuffer.mtext), getpid(), 0) == -1)
        {
            perror("Child: Error with msgrcv\n");
            cleanup();
        }
        char temp[50];
        strcpy(temp, msgBuffer.mtext);
        printf("Child %ld: Message received: \"%s\"\n", proc_id, temp);

        msgBuffer.mtype = proc_id;
        strcpy(msgBuffer.mtext, "Telling oss.c child is done...\0");
        int len = strlen(msgBuffer.mtext);

        if (msgsnd(msgQ_id, &msgBuffer, len+1, 0) == -1)
        {
            perror("oss.c: Error sending message, exiting...\n");
            cleanup();
        }

        break;
    }

    printf("Child %ld: Done receiving messages...\n", proc_id);

    shmdt(processCtrlTable_ptr);
    shmdt(secTimer_ptr);
    shmdt(nsecTimer_ptr);

    return 0;
}