#include "receiver.h"

#define SEM_SEND_SIGNAL "/lab1_SEM_SEND_SIGNAL"
#define SEM_RECV_SIGNAL "/lab1_SEM_RECV_SIGNAL"

#define IPC_FILE_PATH "./LAB1_IPC_FILE"
#define IPC_KEY '1'
#define MESSAGE_SIZE sizeof(message_t)
#define MESSAGE_TYPE 1
#define END_MESSAGE "\x04"

void receive(message_t* message_ptr, mailbox_t* mailbox_ptr){
    /*  
        1. Use flag to determine the communication method
        2. According to the communication method, receive the message
    */
    switch (mailbox_ptr->flag) {
        case MSG_PASSING:  // Message Passing
            msgrcv(mailbox_ptr->storage.msqid, message_ptr, sizeof(message_ptr->msgText), MESSAGE_TYPE, 0);  // error if return -1  // blocking recv
            break;
        case SHARED_MEM:  // Shared Memory
            memcpy(message_ptr, mailbox_ptr->storage.shm_addr, MESSAGE_SIZE);
            break;
        default:
            printf("error: mailbox_ptr->flag == %d\n", mailbox_ptr->flag);
            break;
    }
}

int main(int argc, char* argv[]){
    /*  
        1) Call receive(&message, &mailbox) according to the flow in slide 4
        2) Measure the total receiving time
        3) Get the mechanism from command line arguments
            • e.g. ./receiver 1
        4) Print information on the console according to the output format
        5) If the exit message is received, print the total receiving time and terminate the receiver.c
    */
    if (argc != 2) {
        printf("error: argv == %d\n", argc);
        return 1;
    }

    // init
    int message_method = atoi(argv[1]);
    // Semaphore
    sem_t* send_signal = sem_open(SEM_SEND_SIGNAL, O_CREAT, 0666, 0);  // initial S = 0
    sem_t* recv_signal = sem_open(SEM_RECV_SIGNAL, O_CREAT, 0666, 0);  // initial S = 0
    if (send_signal == SEM_FAILED || recv_signal == SEM_FAILED) {
        printf("error: Semaphore\n");
        return 1;
    }
    // IPC key
    key_t key = ftok(IPC_FILE_PATH, IPC_KEY);
    if (key == -1) {
        printf("error: IPC key\n");
        return 1;
    }
    // IPC
    mailbox_t mailbox = {0};
    int shm_id = 0;
    switch (message_method) {
        case MSG_PASSING:  // Message Passing
            printf("Message Passing\n");
            mailbox.flag = MSG_PASSING;
            mailbox.storage.msqid = msgget(key, IPC_CREAT | 0666);
            if (mailbox.storage.msqid == -1) {
                printf("error: Message Passing [msgget]\n");
                return 1;
            }
            break;
        case SHARED_MEM:  // Shared Memory
            printf("Shared Memory\n");
            mailbox.flag = SHARED_MEM;
            shm_id = shmget(key, MESSAGE_SIZE, IPC_CREAT | 0666);
            if (shm_id == -1) {
                printf("error: Shared Memory [shmget]\n");
                return 1;
            }
            mailbox.storage.shm_addr = shmat(shm_id, NULL, SHM_RDONLY);
            if (mailbox.storage.shm_addr == (void *)-1) {
                printf("error: Shared Memory [shmat]\n");
                return 1;
            }
            break;
        default:
            printf("error: message_method == %d\n", message_method);
            return 1;
    }

    // 3-way hand shaking
    sem_wait(send_signal);  // wait sender hi
    sem_post(recv_signal);  // ack sender
    sem_wait(send_signal);  // wait ack

    // receive and print
    double time_taken = 0;
    while (1) {
        struct timespec start, end;

        sem_wait(send_signal);  // block until send

        // recv
        message_t message = {.mType = MESSAGE_TYPE};
        clock_gettime(CLOCK_MONOTONIC, &start);
        receive(&message, &mailbox);
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // decode
        if (strcmp(message.msgText, END_MESSAGE) == 0) {
            break;  // EOF
        }
        sem_post(recv_signal);  // send signal
        time_taken += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

        // console out
        printf("Receiving message:  %s", message.msgText);
    }
    printf("\nEnd of input file! Exit!\n");
    printf("Total time taken in receiving msg: %f s\n", time_taken);

    // close
    switch (message_method) {
        // struct shmid_ds shm_detail = {0};
        case MSG_PASSING:  // Message Passing
            msgctl(mailbox.storage.msqid, IPC_RMID, NULL);  // error if return -1  // remove message queue immediately
            break;
        case SHARED_MEM:  // Shared Memory
            shmdt(mailbox.storage.shm_addr);  // error if return -1
            shmctl(shm_id, IPC_RMID, NULL);  // error if return -1
            break;
    }
    sem_close(recv_signal);
    sem_unlink(SEM_RECV_SIGNAL);
    return 0;
}
