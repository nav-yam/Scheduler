# SIMPLE SCHEDULER : 
This is implementation of a simple scheduler where user given commands are run in an round-robin scheduling algorithm. In this algorithm, after a time quantum (TSLICE), each running process is stopped and added to the back of the ready queue. In the next cycle, the process in the front are continued for the TSLICE time.


# ASSUMPTIONS :
1. Each command is of maximum 512 characters.
2. User is expected to provide signal (     ) after entering commands, to add them into execution phase.
3. The file provided with submit command is in executable format.


# DESCRIPTION ON THE WORKING: 
The input commands are stored in a linked list AllJobs (in the shared memory pointer) along with their details, like priority, pid, waiting times, execution times, etc. 
Queue stores just the pid of the process to be run and then NCPU ids from this queue are run simulatenously, if unfinished, they are added back to the queue with the same priority. 
In each cycle, queue is sorted on the basis of priority levels.
After the queue is empty, shell and scheduler both are termindated giving out a detailed view of the records history.
For simplicity of the user, we have already hard-coded a few commands for quick execution

# CONTRIBUTION TIMELINE :
Tizil first created a basic framework of the model. 
Navyam then implemented it in the code.
Further several small tasks were distributed thoroughly.
Input management, priority handling, queue/linked list functions were done by Tizil, 
Modifying simpleShell to a single process, creating round robin loops, setting shared memory etc. were done by Navyam.
On the last day we identified a logical bug in the program and we had to spend the whole day together to comeup with an optimal solution that fits our requirement.



# CONTRIBUTORS :
1. Tizil Sharma (2022543)
2. Navyam Garg (2022317)


# REFERENCES : 
Various online articles and gcc-documentations available on the website to gain knowledge of the usage of required APIs