#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"


static char* cur_process_name; /* Current loaded process's name */
static int cur_mem_req; /* Current loaded process's request memory */
static char cur_alloc_flag; /* Current loaded process's allocation method */
static int cur_mem_left = 0; /* Current amount of free memory */
static int initial_mem = 0; /* Starting amount of memory */
static memblock* head = NULL;


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
     * invalid returns 2 or if there isn't enough memory, returns 3.
     */

    char c;
    char process_desc[CMD_MAX_LEN]; /* Temp buffer to hold process_name */
    char requested_mem_str[CMD_MAX_LEN]; /* Temp buffer to hold alloc size */
    int requested_mem = 0; /* Amount of memory requested by user */
    int process_found = 0; /* Flag for whether process arg was found */
    int amount_found = 0; /* Flag for whether amount arg was found */

    if (cmd[0] == 'R') { /* If command starts with RQ or RL */
        if ((cmd[1] == 'Q') || (cmd[1] == 'L')) {
	    if (cmd[2] == ' ') { /* If command separates RQ/RL and process */
		int i = 3;
		int j = 0;
		int k = 0;
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
				process_desc[j] = '\0';
			    }
			    else if (!amount_found) {
				amount_found = 1; //single number found
				requested_mem_str[k] = '\0';
				requested_mem = atoi(requested_mem_str);

				if (requested_mem <= 0) {
	                            fprintf(stderr, "Error: '%s' is not a "
				    "valid memory size.\n", requested_mem_str);
 				    fflush(stderr);
				    return 2;
				}
    				else if (requested_mem > cur_mem_left) {
				    fprintf(stderr, "Error: Cannot allocate %s"
				    " bytes when there is only %d bytes left."
				    "\n", requested_mem_str, cur_mem_left);
				    fflush(stderr);
				    return 3;
				}
			    }
			    else {
				return 2; //more characters after arguments
			    }
			}
		    }
		    else {
			if (!process_found) {
			    process_desc[j++] = cmd[i];
			}
			else if (!amount_found) {
			    requested_mem_str[k++] = cmd[i];
			}
    
			c = cmd[i]; //Show that there is something in string
		    }

		    i++;
		}

                /* Check if command is valid RL or RQ command */
                if ((c != '\0') && (cmd[1] == 'L') && (!process_found)) {
		    /* If valid RL command */
		    process_desc[j] = '\0';
		    cur_process_name = strdup(process_desc);
		    return 1;
		}
		else if ((c != '\0') && (cmd[i-2] == ' ') && (cmd[1] == 'Q')
			   && process_found && amount_found) {
		    /* If valid RQ command */
		    c = cmd[i-1];
		    if ((c == 'F') || (c == 'B') || (c == 'W')) {
			/* If flag is also valid */
			cur_process_name = strdup(process_desc);
			cur_mem_req = requested_mem;
			cur_alloc_flag = c;
		        return 0;
		    }
		    else {
			return 2; //No F, B, or W flag
		    }
		}
		else {
		    return 2; //Not valid RQ/RL command
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

    initial_mem = m_size;
    cur_mem_left = m_size;
}


void requestMemory() {
    /* Allocates a contiguous block of memory to a process, where the process
     * name, memory size, and method are all defined in the string (cmd)
     */

    int cur_block_size = 0; /* Block size of cur block we are looking at */
    int good_req = 0; /* Flag for whether the requested was fulfilled */

    /* Initialize new block */
    memblock* new_mblock = malloc(sizeof(memblock));
    new_mblock->name = cur_process_name;

    /* Initialize trackers and counters for traversing memory */
    memblock* cur_block = head;
    memblock* prev_block = head;
    memblock* best_block = NULL;
    memblock* best_prev_block = NULL;
    int biggest_hole = 0; /* Size of the biggest hole in memory */
    int smallest_hole = 0; /* Size of the smallest hole in memory */

    if (!head) { /* First process being allocated */
	new_mblock->start = 0;
	new_mblock->end = cur_mem_req-1;
	head = new_mblock;
        cur_mem_left -= cur_mem_req;

        if (cur_mem_left != 0) { /* If hole is created due to first process */
	    memblock* empty_mblock = malloc(sizeof(memblock));
	    empty_mblock->name = NULL;
            empty_mblock->start = new_mblock->end + 1;
	    empty_mblock->end = initial_mem - 1;
	    new_mblock->next = empty_mblock;
	}
	else {
	    new_mblock->next = NULL;
	}
    }
    else {
	/* Initialize trackers and counters for traversing memory */
        while (cur_block) {
	    cur_block_size = cur_block->end - cur_block->start + 1;

	    if (cur_block->name == NULL) { /* If hole */
	        if (cur_block_size >= cur_mem_req) { /* If new block fits */
		    if ((cur_alloc_flag == 'B') && (cur_block_size <
			 smallest_hole)) { /* Get smallest hole for best-fit */
		        smallest_hole = cur_block_size;
		     	best_block = cur_block;
		    	best_prev_block = prev_block;
			good_req = 1;
		    }
		    else if ((cur_alloc_flag == 'W') && (cur_block_size >
			 biggest_hole)) { /* Get biggest hole for worst-fit */
		        biggest_hole = cur_block_size;
		     	best_block = cur_block;
		    	best_prev_block = prev_block;
			good_req = 1;
		    }
		    else { /* Save first good hole for first-fit */
			best_block = cur_block;
			best_prev_block = prev_block;
			good_req = 1;
			break;
		    }
		}
	    }

	    /* Move to next memory block */
	    prev_block = cur_block;
	    cur_block = cur_block->next;
        }
    }

    if (good_req) { 
	/* If we found a good hole in populated memory */
        cur_block_size = best_block->end - best_block->start + 1;

	if (cur_block_size == cur_mem_req) { /* No need to make new block */
	    best_block->name = new_mblock->name;
	    free(new_mblock);
	}
	else { /* Insert new block at the beginning of the hole */
	    new_mblock->start = best_block->start;
	    new_mblock->end = new_mblock->start + cur_mem_req - 1;
	    best_block->start = new_mblock->end + 1;
	    new_mblock->next = best_block;

	    if (best_block == head) {
		head = new_mblock;
	    }
	    else {
		best_prev_block->next = new_mblock;
	    }
	}

	cur_mem_left -= cur_mem_req;
    }
    else if (cur_block_size != 0) { /* Was not able to find a hole in memory */
	free(new_mblock);
	fprintf(stderr, "Unable to request memory for process %s.\n",
		cur_process_name);
	fflush(stderr);
    } 
}


void releaseMemory() {
    /* Releases the contiguous block of memory pointed to a process, where
     * the process name is defined in the string (cmd)
     */

    int cur_block_size = 0;
    char* removeMe;
    char* freeName;
    removeMe = cur_process_name;
    memblock* cur_block = head;
    memblock* prev_block = NULL;
    memblock* next_block = NULL;

    /* go through each block to find matching name */
    while (cur_block) {
        cur_block_size = cur_block->end - cur_block->start + 1;
        next_block = cur_block->next;
        if (!strcmp(cur_block->name, removeMe)) { /* if name matches */
            if (next_block) { /* if there is a next block */
                if (prev_block == NULL) { /* if there is no previous block */
                    if (next_block->name == NULL) { /* if next block is hole */
                        /* add to remaining memory */
                        cur_mem_left += cur_block_size;
                        /* combine cur_block and next_block */
                        cur_block->end = next_block->end;
                        cur_block->next = next_block->next;
                        freeName = cur_block->name;
                        cur_block->name = NULL;;
                        free(freeName);
                        free(next_block);
                    }
                    else { /* if next block is not hole */
                        /* add to remaining memory and make cur_block a hole */
                        cur_mem_left += cur_block_size;
                        freeName = cur_block->name;
                        cur_block->name = NULL;
                        free(freeName);
                    }
                }
                else { /* if there is a prev AND next block, check for holes */
                    if (!prev_block->name && !next_block->name) { /* both */
                        /* add to remaining memory */
                        cur_mem_left += cur_block_size;
                        /* combine prev, cur and next into single hole */
                        freeName = cur_block->name;
                        prev_block->end = next_block->end;
                        prev_block->next = next_block->next;
                        free(freeName);
                        free(cur_block);
                        free(next_block);
                    }
                    else if (!prev_block->name && next_block->name) { /*prev*/
                        /* add to remaining memory */
                        cur_mem_left += cur_block_size;
                        /* combine prev_block and cur_block */
                        prev_block->end = cur_block->end;
                        prev_block->next = cur_block->next;
                        freeName = cur_block->name;
                        free(freeName);
                        free(cur_block);
                    }
                    else if (prev_block->name && !next_block->name) { /*next*/
                        /* add to remaining memory */
                        cur_mem_left += cur_block_size;
                        /* combine cur_block and next_block */
                        cur_block->end = next_block->end;
                        cur_block->next = next_block->next;
                        freeName = cur_block->name;
                        cur_block->name = NULL;;
                        free(freeName);
                        free(next_block);
                    }
                    else { /* if prev and next are not holes */
                        /* add to remaining memory and make cur_block a hole */
                        cur_mem_left += cur_block_size;
                        freeName = cur_block->name;
                        cur_block->name = NULL;
                        free(freeName);
                    }
                }
            }
            else { /* if this is the last block */
                if (prev_block == NULL) { /* if this is the only block */
                    /* add to remaining memory and make cur_block a hole */
                    cur_mem_left += cur_block_size;
                    freeName = cur_block->name;
                    cur_block->name = NULL;
                    free(freeName);
                }
                else { /* if there is a previous block */
                    if (prev_block->name == NULL) { /* if prev_block is hole */
                        /* add to remaining memory */
                        cur_mem_left += cur_block_size;
                        /* combine prev_block and cur_block */
                        prev_block->end = cur_block->end;
                        prev_block->next = cur_block->next;
                        freeName = cur_block->name;
                        free(freeName);
                        free(cur_block);
                    }
                    else { /* if prev_block is not hole, make cur_block hole */
                        cur_mem_left += cur_block_size;
                        freeName = cur_block->name;
                        cur_block->name = NULL;
                        free(freeName);
                    }
                }
            }
        }

        prev_block = cur_block;
        cur_block = next_block;
    }

}


void compactMemory() {
    /* Compacts the unused holes of memory into a single contiguous block */
}


void printStatus() {
    /* Prints to stdout the regions of memory that are allocated to processes
     * and the regions of memory that are unused
     */
}
