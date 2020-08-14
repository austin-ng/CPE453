#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"

int main(int argc, char* argv[]) {
    char c;
    int init_size; /* User specified initial amount of allocated memory */
    int line_len; /* Length of the allocator command */
    int err; /* Counter that is incremented when non-exit error occurs */

    /* Check main arguments */
    if (argc != 2) {
	fprintf(stderr, "usage: ./allocator <initial amount of memory>\n");
	exit(EXIT_FAILURE);
    }

    if ((init_size = atoi(argv[1])) == 0) {
	fprintf(stderr, "Error: Initial memory size must be a positive"
			" integer.\n");
	exit(EXIT_FAILURE);
    }

    if (init_size > MAX) {
	fprintf(stderr, "Error: Initial memory size must be less than %d.\n",
		MAX);
	exit(EXIT_FAILURE);
    }

    /* Initialize memory amount and loop variables */
    setMemorySize(init_size);
    err = 0;
    char* cmd = malloc(CMD_MAX_LEN);
    memset(cmd, '\0', CMD_MAX_LEN + 3);

    while (1) {
	printf("allocator>");
	fflush(stdout);

       	if (fgets(cmd, CMD_MAX_LEN + 3, stdin) == NULL) {
            /* Input read error check */
	    fprintf(stderr, "Error occurred while attempting to read\
			     command\n");
	    fflush(stderr);
	    err++;
	}
	else if (strcmp(cmd, "\n") == 0) { 
	    /* Empty command */
	    fprintf(stderr, "Error: Please enter a command\n");
	    fflush(stderr);
	    err++;
	}
        else { 
	    /* Remove \n from fgets result */
	    line_len = strlen(cmd) - 1;
	    cmd[line_len] = '\0';
	}

	if (err == 0) {
	    line_len = strlen(cmd);

            if (line_len > CMD_MAX_LEN) {
		fprintf(stderr, "Command too long. Maximum Length: %d\n",
			CMD_MAX_LEN);
		fflush(stderr);

	        /* Clear out the rest of stdin characters */
		if (line_len > CMD_MAX_LEN + 1) {
	            c = getchar();
	            while ((c != '\n') && (c != EOF)) {
		        c = getchar();
		    }
		}
	    }
	    else {
	        if (line_len == 1) {
	            if (cmd[0] == 'C') { /* If user requested compact */
	    	        compactMemory();
		    }
		    else if (cmd[0] == 'X') { /* User wants to exit */
			freeMemory();
			break;
		    }
		    else {
		        showCommands();
		    }
	        }
	        else if (line_len == 4) {
		    if (strcmp(cmd, "STAT") == 0) { /* If user wants status */
	    	        printStatus();
		    }
		    else {
		        showCommands();
		    }
	        }
	        else if (line_len > 4) {
		    if ((err = validCommand(cmd)) < 2) {
		        if (err == 0) { /* RQ command */
			    requestMemory();
		        }
		        else {
			    releaseMemory();
		        }
		    }
		    else {
			if (err == 2) {
		            showCommands();
			}
		    }
	        }
		else {
		    showCommands();
		}
	    }
	}

	err = 0;
    } 

    return 0;
}
