x#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>

#define CHILD 5
#define MESSAGE  200

int number_child = 0;
pid_t child_a[CHILD];    //array to store the child's PID
int pipe_a[CHILD][2];     //declaring 2D array to store file descriptors of the pipes

sem_t s;

//to print message when the system is interupted (ctrl+c)
void sigint_handler(int sig) {
    printf("\There is some interuption. Exiting to the main page. \n");
    printf("\nPlease try again. \n");
    sem_destroy(&s);
    exit(0);
}

//ask  user to enter a message 
void parent_process() {
    sleep(1);
    fflush(stdin);
    printf("\nplease enter your  message to send to the children: ");
    char message[500];
    if( fgets(message, sizeof(message), stdin) == NULL){
        printf("\nError reading the  input.");
        exit(1);}

    sem_wait(&s);
    for (int a = 0; a < CHILD; a++) {
        printf("Parent is  writing to the child %d\n", a+1);
        write(pipe_a[a][1], message, sizeof(message));
        sleep(1);
    }
    sem_post(&s);
}

void child_process(int id) {
    sem_wait(&s);
    printf("Child %d is reading \n", id+1);

    char message[256];
    read(pipe_a[id][0], message, sizeof(message));
    printf("Child %d received message:%s ", id+1, message);
    sleep(1);
    sem_post(&s);
}

int main() {
    signal(SIGINT, sigint_handler);
    sem_init(&s, 0, 1);

    for (int a = 0; a < CHILD; a++) {
        if (pipe(pipe_a[a]) == -1) {
            printf("Error creating the pipe for the child %d\n", a+1);
            exit(1);
        }
        pid_t pid = fork();
        if (pid == -1) {
            printf("Error forking child %d\n", a+1);
            exit(1);
        } else if (pid == 0) {
            child_process(a);
            exit(0);
        } else {
            child_a[a] = pid;
            number_child++;
            if (a == CHILD-1) {
                parent_process();
            }
        }
    }
    sem_destroy(&s);
    return 0;
}
