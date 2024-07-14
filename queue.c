#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Node
{
    int data;
    int priority;
    struct Node *next;
} Node;

// Define a structure for the queue
typedef struct Queue
{
    Node *front;
    Node *rear;
} Queue;

// Function to create a new node
struct Node *createNode(int data)
{
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = data;
    newNode->priority = 1;
    newNode->next = NULL;
    return newNode;
}
struct Node *createNode2(int data, int pri)
{
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = data;
    newNode->priority = pri;
    newNode->next = NULL;
    return newNode;
}
// print queue
void printQueue(struct Queue *queue)
{
    struct Node *temp = queue->front;
    while (temp != NULL)
    {
        printf("%d ", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

void enqueue(struct Queue *queue, int data, int pri)
{
    struct Node *newNode = createNode2(data, pri);
    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;
    // printQueue(queue);
}


// Function to dequeue an element from the front of the queue
int dequeue(struct Queue *queue)
{
    if (queue->front == NULL)
    {
        printf("Queue is empty.\n");
        exit(1);
    }
    int data = queue->front->data;
    struct Node *temp = queue->front;
    queue->front = queue->front->next;
    free(temp);

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    return data;
}

// sort the queue on the basis of priority
void sortQueue(struct Queue *queue)
{
    struct Node *temp = queue->front;
    struct Node *temp2 = queue->front;
    while (temp != NULL)
    {
        temp2 = temp->next;
        while (temp2 != NULL)
        {
            if (temp->priority > temp2->priority)
            {
                int data = temp->data;
                temp->data = temp2->data;
                temp2->data = data;
                int priority = temp->priority;
                temp->priority = temp2->priority;
                temp2->priority = priority;
            }
            temp2 = temp2->next;
        }
        temp = temp->next;
    }
}

// Function to check if the queue is empty
int isEmpty(struct Queue *queue)
{
    return (queue->front == NULL);
}

// Peek function
int peek(struct Queue *queue)
{
    if (queue->front == NULL)
    {
        printf("Queue is empty.\n");
        exit(1);
    }
    return queue->front->data;
}

// remove element from queue 
void removeElement(struct Queue *queue, int data)
{
    if (queue->front == NULL)
    {
        printf("Queue is empty.\n");
        // exit(1);
    }
    struct Node *temp = queue->front;
    struct Node *prev = NULL;
    while (temp != NULL && temp->data != data)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
    {
        // printf("Element not found. %d\n", data);
        return;
    }
    if (prev == NULL)
    {
        queue->front = queue->front->next;
    }
    else
    {
        prev->next = temp->next;
    }
}

void copyPid(pid_t* arr, int NCPU, struct Queue* q){
    // copy first NCPU elements from queue q to arr, if no more elements are there, add 0
    struct Node *temp = q->front;
    int i = 0, count = 0;
    while (temp != NULL && i < NCPU)
    {
        arr[i] = temp->data;
        temp = temp->next;
        i++;
    }
    count = i;
    while (i < NCPU)
    {
        arr[i] = 0;
        i++;
    }
}