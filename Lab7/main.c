#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_manager.h"

int main(int argc, char* argv[]) {
    FILE* fin;
    int i;
    int phys_addr;
    char cur_addr[MAX_INPUT_LINE_LEN];

    /* Check for usage error */
    if (argc != 2) {
	fprintf(stderr, "usage: ./vm_manager <input text file>\n");
	exit(EXIT_FAILURE);
    }

    /* Check if input file is available to read */
    if ((fin = fopen(argv[1], "r")) == NULL) {
	perror(argv[1]);
	exit(EXIT_FAILURE);
    }

    while (fgets(cur_addr, MAX_INPUT_LINE_LEN, fin) != NULL) {
	phys_addr = get_PAddress(i);
    }

    fclose(fin);

    return 0;
}
