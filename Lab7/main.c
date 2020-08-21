#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_manager.h"

int main(int argc, char* argv[]) {
    FILE* fin;
    FILE* fout;
    char cur_addr[MAX_INPUT_LINE_LEN];
    char result_str[MAX_OUTPUT_LINE_LEN];

    /* Check for usage error */
    if (argc != 2) {
	fprintf(stderr, "usage: ./vmm <input text file>\n");
	exit(EXIT_FAILURE);
    }

    /* Check if input file is available to read */
    if ((fin = fopen(argv[1], "r")) == NULL) {
	perror(argv[1]);
	exit(EXIT_FAILURE);
    }

    /* Check if creating output file runs into an error */
    if ((fout = fopen("vmm_out.txt", "w")) == NULL) {
	perror("vmm_out.txt");
        exit(EXIT_FAILURE);
    }


    init_Manager(); /* Initialize page table and TLB */

    while (fgets(cur_addr, MAX_INPUT_LINE_LEN, fin) != NULL) {
        /* For each logical address */
        int line_len = strlen(cur_addr);
        cur_addr[line_len-1] = '\0';

        /* Get physical address and value at address */
	int phys_addr = get_PAddress(cur_addr);
	signed char val = getValueAt(phys_addr);

        /* Write string to file in correct format */ 
        sprintf(result_str, "Virtual address: %s Physical address: %d " 
			    "Value: %d\n", cur_addr, phys_addr, val);
        line_len = strlen(result_str);
	fwrite(result_str, sizeof(char), line_len, fout);
	printf("%s", result_str);
    }

    printf("(Note: Output also written to file -> vmm_out.txt)\n");

    fclose(fin);
    fclose(fout);

    return 0;
}
