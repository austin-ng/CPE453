#include <stdio.h>
#include <stdlib.h>

#include "allocator.h"


static int cur_mem_left = 0; /* Current amount of free memory */
static int num_processes = 0; /* Number of processes in memory */
static char* processes[MAX_PROCESSES]; /* List of running processes */


void showCommands() {
    /* Displays the syntax for each of the four commands possible in the
     * program
     */

    fprintf(stderr, "Error: Command is invalid. Here are the four possible "
		     "commands:\n");
    fprintf(stderr, "1. 'RQ <process_name> <memory amount> "
		     "<F, B, or W>\n");
    fprintf(stderr, "2. 'RL <process_name>'\n");
    fprintf(stderr, "3. 'C'\n");
    fprintf(stderr, "4. 'STAT'\n\n");

    fflush(stderr);
}


int validCommand(char* cmd) {
    /* Given the current command (cmd), parses through the command and checks
     * whether the command is valid or not. If it is valid, the function
     * returns 0 for a RQ command and 1 for a RL command. If the command is
     * invalid, returns 2
     */

    char c;
    int process_found = 0; /* Flag for whether process arg was found */
    int amount_found = 0; /* Flag for whether amount arg was found */
    int flag_found = 0; /* Flag for whether flag arg was found */

    if (cmd[0] == 'R') { /* If command starts with RQ or RL */
        if ((cmd[1] == 'Q') || (cmd[1] == 'L')) {
	    if (cmd[2] == ' ') { /* If command separates RQ/RL and process */
		int i = 3;
		c = '\0';
		while (cmd[i] != '\0') {
		    /* Walk through the rest of the string and see if there
 		     * is either just a single word in the string (for RL
 		     * command) or a single word, a single number, and a single
 		     * letter
 		     */
		    if (cmd[i] == ' ') {
			if (c != '\0') {
			    c = '\0';
			    if (!process_found) {
				process_found = 1; //single word found
			    }
			    else if (!amount_found) {
				amount_found = 1; //single number found
			    }
			    else if (!flag_found) {
				flag_found = 1; //single letter found
			    }
			    else {
				return 2; //more characters after arguments
			    }
			}
		    }
		    else {
			c = cmd[i]; //Show that there is something in string
		    }

		    i++;
		}

                if ((c != '\0') && (cmd[1] == 'L') && (!process_found)) {
		    /* If valid RL command */
		    return 1;
		}
		else if ((((c == '\0') && flag_found) || ((c != '\0') &&
			    (cmd[i-2] == ' '))) && (cmd[1] == 'Q') && 
			    process_found && amount_found) {
		    /* If valid RQ command */
		    return 0;
		}
		else {
		    return 2;
		}
	    }
	    else {
		return 2; //No space betwen RQ/RL and process name
	    }
	}
	else {
	    return 2; //Didn't start with RQ or RL
	}
    }
    else {
	return 2; //Didn't start with R
    }
}


void setMemorySize(int m_size) {
    /* Sets the global variable which keeps track of the current size of the
     * contiguous region of memory to the integer (m_size). Also initializes
     * other initial variables, including,
     */

    cur_mem_left = m_size;
}


void requestMemory(char* cmd) {
    /* Allocates a contiguous block of memory to a process, where the process
     * name, memory size, and method are all defined in the string (cmd)
     */
    
    char process_desc[CMD_MAX_LEN];
    char requested_mem_str[CMD_MAX_LEN];
    int requested_mem = 0;

    if (num_processes == MAX_PROCESSES) {
	fprintf(stderr, "Unable to add new process. Max number of processes"
			" have been reached.\n");
	fflush(stderr);
	return;
    }

    int i = 2;
    while (cmd[++i] != ' ');

    int j = 0;
    while (cmd[i] != ' ') {
	process_desc[j++] = cmd[i++];
    }
    
    process_desc[j++] = ',';

    while (cmd[i++] != ' ');

    int k = 0;
    while (cmd[i] != ' ') {
        requested_mem_str[k++] = cmd[i];   
	process_desc[j++] = cmd[i++];
    }

    requested_mem_str[k] = '\0';
    process_desc[j] = '\0';

    requested_mem = atoi(requested_mem_str);

    if (requested_mem == 0) {
	fprintf(stderr, "Error: '%s' is not a valid memory size.\n",
		requested_mem_str);
 	fflush(stderr);
	return;
    }
    else if (requested_mem > cur_mem_left) {
	fprintf(stderr, "Error: Cannot allocate %s bytes when there is only"
			" %d bytes left.\n", requested_mem_str, cur_mem_left);
	fflush(stderr);
	return;
    }
    else {
	cur_mem_left -= requested_mem;
	processes[num_processes] = process_desc;
    }
}


void releaseMemory(char* cmd) {
    /* Releases the contiguous block of memory pointed to a process, where
     * the process name is defined in the string (cmd)
     */
}


void compactMemory() {
    /* Compacts the unused holes of memory into a single contiguous block */
}


void printStatus() {
    /* Prints to stdout the regions of memory that are allocated to processes
     * and the regions of memory that are unused
     */
}
