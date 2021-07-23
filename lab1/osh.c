#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#define MAX_LINE 80


void parseInput(char* command, char** arg) {
    int count = 0; /* index for arg */
    int i = 0; /* index for command line */
    int j = 0; /* index for cur_cmd*/
    char cur_cmd[MAX_LINE + 1];
    memset(cur_cmd, '\0', MAX_LINE + 1);
    while (command[i] != '\0') {
        if (command[i] == ' ') {
            /* allocate memory for new command and put into args */
            if (cur_cmd[0] != '\0') {
                char* new_cmd = malloc(strlen(cur_cmd) + 1);
                strcpy(new_cmd, cur_cmd);
                *(arg + count++) = new_cmd;
            }
            memset(cur_cmd, '\0', MAX_LINE + 1);
            j = 0;
        }

	else if (command[i] == '|') {
	    if (cur_cmd[0] != '\0') {
		char* c1 = malloc(strlen(cur_cmd) + 1);
		strcpy(c1, cur_cmd);
		*(arg + count++) = c1;
	    }
	    *(arg + count++) = "|";
	    memset(cur_cmd, '\0', MAX_LINE + 1);
	    j = 0;
	}

	else if (command[i] == '<') {
            if (cur_cmd[0] != '\0') {
                char* c2 = malloc(strlen(cur_cmd) + 1);
                strcpy(c2, cur_cmd);
                *(arg + count++) = c2;
            }
            *(arg + count++) = "<";
            memset(cur_cmd, '\0', MAX_LINE + 1);
            j = 0;
	}

	else if (command[i] == '>') {
            if (cur_cmd[0] != '\0') {
                char* c3 = malloc(strlen(cur_cmd) + 1);
                strcpy(c3, cur_cmd);
                *(arg + count++) = c3;
            }
            *(arg + count++) = ">";
            memset(cur_cmd, '\0', MAX_LINE + 1);
            j = 0;
	}

        else {
            cur_cmd[j++] = command[i];
        }
        i++;
    }
    if (cur_cmd[0] != '\0') {
        char* last_cmd = malloc(strlen(cur_cmd) + 1);
        strcpy(last_cmd, cur_cmd);
        *(arg + count++) = last_cmd;
    }
}


void runCommand(char** argmnts) {

    /* new argv with only commands (no |, <, or >) */
    char *cmds[MAX_LINE/2 + 1];
    memset(cmds, '\0', MAX_LINE / 2 + 1);
    /* booleans: 1 if |, < or > present */
    int pipeOn = 0;
    int rdIn = 0;
    int rdOut = 0;
    int runConc = 0;
    int i = 0;
    int j = 0;
    char* cur_arg;
    int pipeIndex;
    /* name of input/output files */
    char inFile[MAX_LINE + 1];
    char outFile[MAX_LINE + 1];
    memset(inFile, '\0', MAX_LINE + 1);
    memset(outFile, '\0', MAX_LINE + 1);


    int numargs = 0;
    while (argmnts[numargs]) numargs++;
    int res; /* for chdir */
    if (strcmp(argmnts[0], "cd") == 0) {
	if (numargs < 2) {
	    printf("cd: missing argument.\n");
	    exit(EXIT_FAILURE);
	}
	else if (numargs > 2) {
	    printf("cd: too many arguments.\n");
	    exit(EXIT_FAILURE);
	}
	else {
	    res = chdir(argmnts[1]);
	    if (res != 0) {
		perror(argmnts[1]);
		exit(EXIT_FAILURE);
	    }
	    return;
	}
    }



    while ((cur_arg = argmnts[i])) {
	if (strcmp(cur_arg, "|") == 0) {
	    pipeOn = 1;
	    /* set pipeIndex to first command after pipe */
	    pipeIndex = i;
	}
	else if (strcmp(cur_arg, "<") == 0) {
	    rdIn = 1;
	    strcpy(inFile, argmnts[i + 1]);
	    i++;
	}
	else if (strcmp(cur_arg, ">") == 0) {
	    rdOut = 1;
	    strcpy(outFile, argmnts[i + 1]);
	    i++;
	}
	else if (strcmp(cur_arg, "&") == 0) {
	    runConc = 1;
	}
	else {
	    cmds[j++] = cur_arg;
	}
	i++;
    }

/* delete later: print cmds
    char* curcmd;
    int k = 0;
    while ((curcmd = cmds[k])) {
        printf("%s\n", curcmd);
        k++;
    }
*/


    /* second set of commands that read from pipe */
    char *cmds2[MAX_LINE/2 + 1];
    memset(cmds2, '\0', MAX_LINE / 2 + 1);
    int cnt = 0;

    if (pipeOn == 1) {
	while (cmds[pipeIndex]) {
	    cmds2[cnt++] = cmds[pipeIndex];
	    cmds[pipeIndex] = '\0';
	    pipeIndex++;
	}
/* delete later: print cmds
	int a = 0;
	int b = 0;
	printf("START OF CMDS\n");
	while (cmds[a]) {
	    printf("%s\n", cmds[a]);
	    a++;
	}
	printf("END OF CMDS, START OF CMDS2\n");
	while (cmds2[b]) {
	    printf("%s\n", cmds2[b]);
	    b++;
	}
	printf("END OF CMDS2\n");
*/
    }

    int f_in = STDIN_FILENO;
    if (rdIn == 1) {
	if ((f_in = open(inFile, O_RDONLY)) < 0) {
	    perror("<");
	    exit(EXIT_FAILURE);
	}
    }

    int f_out = STDOUT_FILENO;
    if (rdOut == 1) {
	if ((f_out = open(outFile, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
	    perror(">");
	    exit(EXIT_FAILURE);
	}
    }

    pid_t child;
    int w_status;


    if (pipeOn == 1) {
	int p[2];
	if (pipe(p) < 0)
	    exit(1);
	if ((child = fork())) {
	    if (child == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	    }
	    else {
                if (wait(&w_status) == -1) {
                    perror("wait");
		    exit(EXIT_FAILURE);
		}
		close(p[1]);
	    }
	}
	else {
	    if (dup2(p[1], STDOUT_FILENO) == -1) {
		perror("pipe write");
		exit(EXIT_FAILURE);
	    }
	    else {
		close(p[0]);
		close(p[1]);
		execvp(cmds[0], cmds);
		perror("execvp");
		exit(EXIT_FAILURE);
	    }
	}
        if ((child = fork())) {
            if (child == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else {
                if (wait(&w_status) == -1) {
                    perror("wait");
                    exit(EXIT_FAILURE);
                }
                close(p[0]);
		return;
            }
        }
        else {
            if (dup2(p[0], STDIN_FILENO) == -1) {
                perror("pipe read");
                exit(EXIT_FAILURE);
            }
            else {
                close(p[0]);
                close(p[1]);
                execvp(cmds2[0], cmds2);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
    }



    if (runConc == 1) {
	/* parent and child run concurrently */
	if ((child = fork())) {
	    if (child == -1) {
		perror("fork");
	    }
	    else {
                if (rdIn) close(f_in);
                if (rdOut) close(f_out);
		return;
	    }
	}
	else {
	    /* if there are redirects, dup stdin/stdout */
	    if (f_in != STDIN_FILENO) {
		if (dup2(f_in, STDIN_FILENO) == -1) {
		    perror("stdin dup");
		    exit(EXIT_FAILURE);
		}
		else {
		    // do nothing
		}
	    }
	    else if (f_out != STDOUT_FILENO) {
		if (dup2(f_out, STDOUT_FILENO) == -1) {
		    perror("stdout dup");
		    exit(EXIT_FAILURE);
		}
		else {
		    // do nothing
		}
	    }
	    execvp(cmds[0], cmds);
	    perror(cmds[0]);
	    exit(EXIT_FAILURE);
	}
    }
    else {
	/* parent process waits */
	if ((child = fork())) {
	    if (child == -1) {
		perror("fork");
	    }
	    else {
		if (wait(&w_status) == -1) {
		    perror("wait");
		    return;
		}
		if (rdIn) close(f_in);
		if (rdOut) close(f_out);
		return;
	    }
	}
	/* child process executes program */
	else {
            /* if there are redirects, dup stdin/stdout */
            if (f_in != STDIN_FILENO) {
                if (dup2(f_in, STDIN_FILENO) == -1) {
                    perror("stdin dup");
                    exit(EXIT_FAILURE);
                }
                else {
		    //do nothing  
                }
            }
            else if (f_out != STDOUT_FILENO) {
                if (dup2(f_out, STDOUT_FILENO) == -1) {
                    perror("stdout dup");
                    exit(EXIT_FAILURE);
                }
                else {
		    //do nothing
                }
            }
	    execvp(cmds[0], cmds);
	    perror(cmds[0]);
	    exit(EXIT_FAILURE);
	}
    }
}




int main(void) {

    char *args[MAX_LINE / 2 + 1]; /* command line args */
    int should_run = 1; /* flag to determine when to exit prog */
    char cmd_line[MAX_LINE + 1]; /* buffer for command line */
    char hist[MAX_LINE + 1]; /* history buffer */
    int i = 0;
    char* curcmd;

    memset(args, '\0', MAX_LINE / 2 + 1);
    /* fill command w empty string by default */
    memset(cmd_line, '\0', MAX_LINE + 1);
    memset(hist, '\0', MAX_LINE + 1);
    while (should_run) {
        printf("osh> ");
        if (fgets(cmd_line, MAX_LINE, stdin) == NULL) {
        	should_run = 0;
        }
        else {
	    /* remove trailing newline and parse command into args */
            int len = strlen(cmd_line);
            if (cmd_line[len - 1] == '\n') 
                cmd_line[len-1] = '\0';
	    /* save to history buffer */
            parseInput(cmd_line, args);
	    /* delete later: print every command in args */
/*
            while ((curcmd = args[i])) {
                printf("%s\n", curcmd);
		if (strcmp(args[i], "|") != 0 &&
		    strcmp(args[i], "<") != 0 &&
		    strcmp(args[i], ">") != 0)
                    free(args[i]);
		i++;
            }
*/
	    /* end delete later */
	    if (strcmp(args[0], "exit") == 0) {
		should_run = 0;
	    }
	    else if (strcmp(args[0], "!!") == 0) {
	        /* run history feature */
		if (hist[0] == '\0') {
		    printf("No commands in history.\n");
		}
		else {
		    /* print history */
		    printf("%s\n", hist);
		    /* run command */
		    parseInput(hist, args);
		    runCommand(args);
		}
	    }
	    else {
		/* run the program */
		runCommand(args);
		/* save to history */
		strcpy(hist, cmd_line);
	    }
            i = 0;
            memset(args, '\0', MAX_LINE / 2 + 1);
            memset(cmd_line, '\0', MAX_LINE + 1);
        }
        fflush(stdout);
    }
    printf("exiting program...\n");
    return 0;

}

