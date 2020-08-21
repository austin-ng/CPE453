#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_manager.h"


static char page_table[NUM_PAGES][PAGE_SIZE];


int get_PAddress(char* addr) {
    /* Takes a string representing a 32-bit logical address (addr) and
     * converts that into a 16-bit integer representing its physical
     * address
     */
 
    int p_addr; /* Physical address from translated logical address */

    /* Convert 32-bit integer string into a 16-bit integer */
    int new_addr32 = atoi(addr);
    int new_addr16 = new_addr32 & BOTH_BYTES_MASK;

    if ((new_addr16 == 0) && (addr[0] == '0')) {
	fprintf(stderr, "Unable to add logical address '%s' into manager\n",
			addr);
	return -1;
    }

    int page_nbr = new_addr16 >> SHIFT_BYTE;
    int page_offs = new_addr16 & LOW_BYTE_MASK;

    if (page_table[page_nbr]) {
	if (page_table[page_nbr]) {
	}
    }

    return p_addr;
}
