#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// This function interperts the str and update values in the attributes, command, attr_num variables

// Creates child process to execute command stored in the attributes array and simulatneously checks the execution status
int launch(char inpt[512])
{
    // Forks and creates child process
    // Replaces newline character in the string with null character

    char str[512];
    strcpy(str, inpt);
    char *attributes[16];
    char *buffer;
    if (str[strlen(str)] == '\n')
        str[strlen(str)] = '\0';
    if (str[strlen(str) - 1] == '\n')
        str[strlen(str) - 1] = '\0';

    // Split strings for whitespace

    buffer = strtok(str, " ");

    if (!buffer)
    {
        printf("\033[1;31mError while parsing the commands!\n\033[0m");
    }

    char *command = buffer;
    int attr_num = 0;

    // Split strings for whitespace
    attributes[attr_num++] = command;
    while ((buffer = strtok(NULL, " ")) != NULL)
    {
        attributes[attr_num++] = buffer;
    }
    attributes[attr_num] = NULL;
    printf("EXECUTING %s at %d\n", attributes[0], getpid());
    if (strcmp(attributes[0], "submit") == 0)
    {
        // ADD submit imoplementation
        execl(attributes[1], attributes[1], NULL);
        perror("\033[1;31mERROR : \033[0m"); // [ ERROR HANDLING ]
        exit(25);
    }

    // If command is none of the above, execute it normally.
    else
    {
        execvp(command, attributes);

        perror("\033[1;31mERROR : \033[0m"); // [ ERROR HANDLING ]
        exit(25);
    }
    return 25;
}