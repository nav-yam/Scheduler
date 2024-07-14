#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

typedef struct record
{
    char command[512];
    int pid;
    int execution_time;
    int priority;
    struct timeval arrival_time;
    struct timeval start_time;
    struct timeval completion_time;
    struct timeval completion_time2;
} record;

typedef struct LinkedListNode
{
    record *data;
    struct LinkedListNode *next;
    int isExecuted;
    int isCompleted;
} LinkedListNode;

typedef struct LinkedList
{
    LinkedListNode *front;
    LinkedListNode *rear;
} LinkedList;

void AddRecordToLinkedList(record *r, LinkedList *l)
{
    LinkedListNode *lln = (LinkedListNode *)malloc(sizeof(LinkedListNode));
    lln->data = r;
    lln->next = NULL;
    lln->isExecuted = 0;
    lln->isCompleted = 0;

    if (l->rear == NULL)
    {
        l->front = l->rear = lln;
    }
    else
    {
        l->rear->next = lln;
        l->rear = l->rear->next;
    }
}

record *newRecord(char *commnd)
{
    record *new = (record *)malloc(sizeof(record));
    strcpy(new->command, commnd);
    new->execution_time = 0;
    gettimeofday(&new->arrival_time, NULL);
    return new;
}



long long get_time_in_mill_sec(struct timeval tv)
{
    long long milliseconds = (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
    return milliseconds;
}
LinkedListNode *search(LinkedList *Alljobs, int pid)
{
    LinkedListNode *current = Alljobs->front;

    while (current != NULL)
    {
        if (current->data->pid == pid)
        {
            return current; // Return the Node where the integer was found
        }
        current = current->next;
    }

    return NULL; // Return NULL if the integer was not found in the lis
}

void displayRecord(record *rec)
{
    int num = get_time_in_mill_sec(rec->completion_time) - get_time_in_mill_sec(rec->completion_time2);
    printf("%d\t %d\t %s\t %d\t %lld\n",rec->priority, rec->pid, rec->command, rec->execution_time + (num/2), get_time_in_mill_sec(rec->completion_time) - get_time_in_mill_sec(rec->arrival_time) - (int)rec->execution_time);
    // printf("%lld\t %lld\t %lld\n", get_time_in_mill_sec(rec->arrival_time), get_time_in_mill_sec(rec->start_time), get_time_in_mill_sec(rec->completion_time) - get_time_in_mill_sec(rec->completion_time2));
}

int searchByAddress(struct LinkedListNode *head, struct LinkedListNode *target)
{
    struct LinkedListNode *current = head;
    int i = 0;

    while (current != NULL)
    {
        if (current == target)
        {
            return i; // Found the node with the target address
        }
        current = current->next;
        i++;
    }

    return -1; // Node with the target address not found
}

void AddToList(LinkedList *Alljobs, char **inpts, int nm)
{
    for (int i = 0; i < nm; i++)
    {
        AddRecordToLinkedList(newRecord(inpts[i]), Alljobs);
    }

    return;
}

// return the i th node in the linked list
LinkedListNode *getithNode(LinkedList *Alljobs, int i)
{
    LinkedListNode *current = Alljobs->front;
    int j = 0;
    while (current != NULL)
    {
        if (j == i)
        {
            return current; // Return the Node where the integer was found
        }
        current = current->next;
        j++;
    }

    return NULL; // Return NULL if the integer was not found in the lis
}

void displayElement(LinkedListNode *nd)
{
    printf("%s\t %d\t %d\t %d\t\n", nd->data->command, nd->isExecuted, nd->isCompleted, nd->data->pid);
}

// input a string array and a pointer to linked list
int addToList(char **inpts, LinkedList *Alljobs)
{
    int i = 0;
    while (inpts[i] != NULL)
    {
        printf("ADDDDING : %s\n", inpts[i]);
        AddRecordToLinkedList(newRecord(inpts[i]), Alljobs);
    }
    return i;
}

// sort linked list on the priority
void sort(LinkedList *Alljobs)
{
    LinkedListNode *current = Alljobs->front;
    LinkedListNode *index = NULL;
    record *temp;
    if (Alljobs->front == NULL)
    {
        return;
    }
    else
    {
        while (current != NULL)
        {
            index = current->next;

            while (index != NULL)
            {
                if (current->data->priority > index->data->priority)
                {
                    temp = current->data;
                    current->data = index->data;
                    index->data = temp;
                }
                index = index->next;
            }
            current = current->next;
        }
    }
}