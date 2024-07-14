#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "queue.c"
#include "ll.c"
#include "simpleShell.c"
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
// #include "dummy_main.c"

int NCPU, TSLICE;
void check(char *i)
{
    printf("OK %s \n", i);
}
struct SharedData
{
    LinkedList *Alljobs;
    struct Queue *queue;
    char message[512];
    sem_t sem;
};
int shm_fd;
struct SharedData *shared_memory_ptr;

struct SharedData *setupSHM()
{
    sem_init(&shared_memory_ptr->sem, 1, 1);
    const char *shared_memory_name = "/my_shared_memory";
    const int shared_memory_size = sizeof(struct SharedData);
    shm_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(shm_fd, shared_memory_size) == -1)
    {
        perror("ftruncate error");
        exit(1);
    }

    // Map the shared memory object into the process's address space
    struct SharedData *shared_memory_ptr = (struct SharedData *)mmap(NULL, shared_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shared_memory_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    return shared_memory_ptr;
}
// Cleanup function for shared memory openend in setupSHM

void cleanupSHM(struct SharedData *shared_memory_ptr)
{

    const char *shared_memory_name = "/my_shared_memory";
    const int shared_memory_size = sizeof(struct SharedData);
    if (munmap(shared_memory_ptr, shared_memory_size) == -1)
    {
        perror("munmap");
        exit(1);
    }
    if (shm_unlink(shared_memory_name) == -1)
    {
        perror("shm_unlink");
        exit(1);
    }
    // Close shm_fd
    if (close(shm_fd) == -1)
    {
        perror("close");
        exit(1);
    }
    sem_destroy(&shared_memory_ptr->sem);
}
void setithNode(struct SharedData *shm, int i, int id)
{
    LinkedListNode *current = shm->Alljobs->front;
    int j = 0;
    while (current != NULL)
    {
        if (j == i)
        {
            current->data->pid = id;
            current->isExecuted = 1;
            gettimeofday(&current->data->start_time, NULL);
            return; // Return the Node where the integer was found
        }
        current = current->next;
        j++;
    }

    return; // Return NULL if the integer was not found in the lis
}

void sigchld_handler(int signo)
{
    pid_t child_pid;
    int status;

    // Reap child processes that have terminated
    while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            LinkedListNode *chld = search(shared_memory_ptr->Alljobs, child_pid);
            if (chld == NULL)
                return;
            gettimeofday(&chld->data->completion_time, NULL);
            chld->isCompleted = 1;
            removeElement(shared_memory_ptr->queue, child_pid);
        }
    }
}
char cmnd[10][512];
void customHandler(int signo)
{
    addToList(cmnd, shared_memory_ptr->Alljobs);
    return;
    // skip interrupt signal
}
int main()
{
    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, customHandler);

    shared_memory_ptr = setupSHM();
    printf("\033[1;31m\t STARTING THE PROGRAM\n\033[0m");

    struct Queue *ready_queue = (struct Queue *)malloc(sizeof(struct Queue));

    // Initialise the Linked List storing details of each command
    LinkedList *AllJobs = (LinkedList *)malloc(sizeof(LinkedList));
    shared_memory_ptr->Alljobs = AllJobs;
    shared_memory_ptr->queue = ready_queue;
    if (shared_memory_ptr->queue == NULL || shared_memory_ptr->Alljobs == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    // Base Case
    shared_memory_ptr->queue->front = shared_memory_ptr->queue->rear = NULL;
    shared_memory_ptr->Alljobs->front = shared_memory_ptr->Alljobs->rear = NULL;
    AddRecordToLinkedList(newRecord("submit hello 2"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit fib 1"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit fib 4"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit hello 4"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit hello 2"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit hello 3"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit hello 1"), shared_memory_ptr->Alljobs);
    // AddRecordToLinkedList(newRecord("submit infi"), shared_memory_ptr->Alljobs);
    // AddRecordToLinkedList(newRecord("submit ini"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit fib 5"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit hello 2"), shared_memory_ptr->Alljobs);
    AddRecordToLinkedList(newRecord("submit hello"), shared_memory_ptr->Alljobs);
    printf("Enter Number of CPU resources : ");
    scanf("%d", &NCPU);

    printf("Enter Time quantum (in ms) : ");
    scanf("%d", &TSLICE);
    int main_pid = fork();

    // Error Handling
    if (main_pid < 0)
    {
        perror("Error Forking");
    }

    // Simple Shell
    else if (main_pid == 0)
    {
        printf("\033[1;31m\t Shell Starting\n\033[0m");
        while (1)
        {
            for (int i = 0; i < 1; i++)
            {
                // reset string at cmnd[i]
                memset(cmnd[i], 0, sizeof(cmnd[i]));
            }

            for (int i = 0; i < 1; i++)
            {
                printf("Enter a command : ");
                char input[100];
                if (fgets(input, sizeof(input), stdin) != NULL)
                {
                    if (strcmp(input, "exit\n") == 0)
                    {
                        break; // Exit the program
                    }
                    strcpy(cmnd[i], input);
                }
            }
            if(sem_wait(&shared_memory_ptr->sem) == -1){
                perror("sem_wait");
            }
            addToList(cmnd, shared_memory_ptr->Alljobs);
            if (sem_post(&shared_memory_ptr->sem) == -1)
            {
                perror("sem_post");
            }
        }
        exit(0);
    }
    else
    {

        printf("\033[1;31m\t Scheduler Starting\n\033[0m");
        // Current points to the command that have not been initialised yet
        LinkedListNode *current = shared_memory_ptr->Alljobs->front;
        int current_indx = 0;

        // After fork, child uses this variable to update all the details of running command
        LinkedListNode *process_lln;
        int cycle_num = 1;
        pid_t pidOfNCPU[NCPU];
        pid_t pidOfExecution[NCPU];

        int job_pid, process;
        current = shared_memory_ptr->Alljobs->front;

        // If queue is empty and no new command is received, loop breaks
        while ((current != NULL) || !isEmpty(shared_memory_ptr->queue))
        {
            sem_wait(&shared_memory_ptr->sem);
            printf("\033[1;31m\tCycle : %d\n\033[0m", cycle_num);
            current = shared_memory_ptr->Alljobs->front;
            while (current != NULL)
            {
                while (current != NULL && current->isExecuted == 1)
                {
                    current = current->next;
                }
                if (current == NULL || current->isExecuted)
                    break;
                int priority = 1;
                // if last non white-space character is integer = priority
                if (current->data->command[strlen(current->data->command) - 1] >= '0' && current->data->command[strlen(current->data->command) - 1] <= '9')
                {
                    priority = atoi(&current->data->command[strlen(current->data->command) - 1]);
                    current->data->command[strlen(current->data->command) - 1] = '\0';
                }
                job_pid = fork();
                // Error Handling
                if (job_pid < 0)
                {
                    perror("Error forking");
                }
                // Child will execute the command
                else if (job_pid == 0)
                {
                    launch(current->data->command);
                }
                else
                {

                    // Signal job_pid to STOP
                    if (kill(job_pid, SIGSTOP) != 0)
                    {
                        perror("Error in stopping the process");
                    };
                    printf("%d\n", job_pid);
                    current->data->pid = job_pid;
                    current->data->priority = priority;

                    gettimeofday(&current->data->start_time, NULL);
                    current->isExecuted = 1;
                    process = job_pid;
                    enqueue(shared_memory_ptr->queue, process, priority);

                    printf("PROCESS STOPPED N BEGGINING : %d : %s\n", kill(job_pid, SIGSTOP), current->data->command);
                    current = current->next;
                }
            }

            if (isEmpty(shared_memory_ptr->queue))
                break;
            sortQueue(shared_memory_ptr->queue);
            printQueue(shared_memory_ptr->queue);
            for (int i = 0; i < NCPU; i++)
            {
                copyPid(pidOfExecution, NCPU, shared_memory_ptr->queue);
                pidOfNCPU[i] = 0;
            }
            for (int i = 0; i < NCPU; i++)
            {

                printf("\033[1;31mRunning %d/%d Process in Cycle : %d\n\033[0m", i + 1, NCPU, cycle_num);

                // Move to the command that needs to be executed
                current_indx = searchByAddress(shared_memory_ptr->Alljobs->front, current);
                int process, job_pid;

                if ((current == NULL || current->isExecuted == 1))
                {
                    if (isEmpty(shared_memory_ptr->queue))
                        break;
                }
                process = pidOfExecution[i];
                if (process == 0)
                    continue;
                int oneOfNCPU = fork();
                if (oneOfNCPU == 0)
                {

                    process_lln = search(shared_memory_ptr->Alljobs, process);
                    if (process_lln == NULL)
                    {
                        printf("ERROR : Process not found in the linked list\n");
                        exit(0);
                    }
                    process_lln->isExecuted = 1;
                    // Signal the process to continue
                    if (kill(process, SIGCONT) != 0)
                    {
                        perror("Error in continuing the process");
                    }

                    // Wait for TSLICE seconds and simultaneously check if the process is still running
                    int i = 0, result;
                    while (i < TSLICE * 10)
                    {
                        // Sleep for 0.1 second
                        usleep(100);
                        // Check if the process is still running
                        int status;
                        // If process is still running, increment i
                        process_lln = search(shared_memory_ptr->Alljobs, process);
                        if (process_lln->isCompleted)
                        {
                            check("YESSSS\n");
                            break;
                        }
                        else
                        {
                            i++;
                        }
                    }
                    kill(process, SIGSTOP);
                    exit(1);
                }
                else
                {
                    pidOfNCPU[i] = oneOfNCPU;

                    if (current != NULL)
                    {
                        current = current->next;
                    }
                }
            }
            cycle_num++;

            // Wait for all the processes to complete
            for (int i = 0; i < NCPU; i++)
            {
                if (pidOfNCPU[i] == 0 || pidOfExecution[i] == 0)
                    continue;
                int status;
                waitpid(pidOfNCPU[i], &status, 0);
                process_lln = search(shared_memory_ptr->Alljobs, pidOfExecution[i]);
                process_lln->data->execution_time += TSLICE;
                if (process_lln->isCompleted == 1)
                    gettimeofday(&process_lln->data->completion_time2, NULL);
            }

            if (isEmpty(shared_memory_ptr->queue))
            {
                printf("Queue is empty\n");
            }
            sem_post(&shared_memory_ptr->sem);
        }
    }
    printf("\033[1;31m\tAll commands successfully executed \n\033[0m");
    LinkedListNode *rec = shared_memory_ptr->Alljobs->front;
    printf("Prio\t PID\t COMMAND\t Execution TIME\t Wait TIME\n");
    while (rec != NULL)
    {
        displayRecord(rec->data);
        rec = rec->next;
    }
    printf("\n\n*******************************************************\n\n");
    sort(shared_memory_ptr->Alljobs);
    rec = shared_memory_ptr->Alljobs->front;
    int prev = 1;
    printf("Prio\t PID\t COMMAND\t Execution TIME\t Wait TIME\n");
    while (rec != NULL)
    {
        if(rec->data->priority != prev){
            printf("\n");
            prev = rec->data->priority;
        }
        displayRecord(rec->data);
        rec = rec->next;
    }
    kill(main_pid, SIGKILL);
    printf("\033[1;31m\t Terminating Schell\n\033[0m");
    printf("\033[1;31m\t Terminating Scheduler\n\033[0m");
    cleanupSHM(shared_memory_ptr);
}
