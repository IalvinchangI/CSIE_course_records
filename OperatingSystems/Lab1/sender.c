#include "sender.h"

#define SEM_SEND_SIGNAL "/lab1_SEM_SEND_SIGNAL"
#define SEM_RECV_SIGNAL "/lab1_SEM_RECV_SIGNAL"

#define IPC_FILE_PATH "./LAB1_IPC_FILE"
#define IPC_KEY '1'
#define MESSAGE_SIZE sizeof(message_t)
#define MESSAGE_TYPE 1
#define END_MESSAGE "\x04"

void send(message_t message, mailbox_t* mailbox_ptr){
    /*  
        1. Use flag to determine the communication method
        2. According to the communication method, send the message
    */
    switch (mailbox_ptr->flag) {
        case MSG_PASSING:  // Message Passing
            msgsnd(mailbox_ptr->storage.msqid, &message, sizeof(message.msgText), 0);  // error if return -1  // blocking send
            break;
        case SHARED_MEM:  // Shared Memory
            memcpy(mailbox_ptr->storage.shm_addr, &message, MESSAGE_SIZE);
            break;
        default:
            printf("error: mailbox_ptr->flag == %d\n", mailbox_ptr->flag);
            break;
    }
}

static double send_post_timing(message_t message, mailbox_t* mailbox_ptr, sem_t* signal) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    send(message, mailbox_ptr);
    clock_gettime(CLOCK_MONOTONIC, &end);
    sem_post(signal);  // send signal
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
}

int main(int argc, char* argv[]){
    /*  
        1) Call send(message, &mailbox) according to the flow in slide 4
        2) Measure the total sending time
        3) Get the mechanism and the input file from command line arguments
            • e.g. ./sender 1 input.txt
                    (1 for Message Passing, 2 for Shared Memory)
        4) Get the messages to be sent from the input file
        5) Print information on the console according to the output format
        6) If the message form the input file is EOF, send an exit message to the receiver.c
        7) Print the total sending time and terminate the sender.c
    */
    if (argc != 3) {
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
            mailbox.storage.shm_addr = shmat(shm_id, NULL, 0);  // read and write
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
    sem_post(send_signal);  // hi receiver
    sem_wait(recv_signal);  // wait ack
    sem_post(send_signal);  // ack receiver

    // read and send
    double time_taken = 0;
    FILE* file = fopen(argv[2], "r");
    while (1) {
        // read
        message_t message = {.mType = MESSAGE_TYPE};
        if (fgets(message.msgText, sizeof(message.msgText), file) == NULL) {
            strcpy(message.msgText, END_MESSAGE);
            time_taken += send_post_timing(message, &mailbox, send_signal);
            break;  // EOF
        }
        
        // send
        time_taken += send_post_timing(message, &mailbox, send_signal);

        // console out
        printf("Sending message:  %s", message.msgText);

        sem_wait(recv_signal);  // block until recv
    }
    printf("\nEnd of input file! Exit!\n");
    printf("Total time taken in sending msg: %f s\n", time_taken);

    // close
    fclose(file);
    switch (message_method) {
        // struct shmid_ds shm_detail = {0};
        case MSG_PASSING:  // Message Passing
            // msgctl(mailbox.storage.msqid, IPC_RMID, NULL);  // error if return -1  // remove message queue immediately
            break;
        case SHARED_MEM:  // Shared Memory
            shmdt(mailbox.storage.shm_addr);  // error if return -1
            shmctl(shm_id, IPC_RMID, NULL);  // error if return -1
            break;
    }
    sem_close(send_signal);
    sem_unlink(SEM_SEND_SIGNAL);
    return 0;
}
