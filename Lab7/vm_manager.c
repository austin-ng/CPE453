#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_manager.h"


static int page_table[NUM_PAGES]; /* Buffer for page table holding frame #'s */
static signed char physical_mem[NUM_FRAMES][FRAME_SIZE]; /* Physical Memory */
static int frame_counter = 0; /* Frame counter for making page entries */


void init_Manager() {
    /* Sets all the entries in the page table to -1, signifying that the
     * page is not there, leading to a page fault. Also sets all the entries
     * in the TLB to -1 page # and -1 frame #, to signify that there is nothing
     * in the TLB yet.
     */

    int i;

    for (i = 0; i < NUM_PAGES; i++) {
	page_table[i] = -1;
    }
}

int from_TLB(int pageno) {
    /* Takes the page number from the logical address (pageno) and returns
     * the frame number for that page if it appears in the TLB or returns
     * -1 if it is not
     */

    return -1;
}


void update_TLB(int pageno, int frameno) {
    /* Takes a page number (pageno) and its corresponding frame number
     * (frameno) from the page table and updates the TLB using FIFO to
     * rearrange its contents
     */
}


int from_PageTable(int pageno) {
    /* Takes the page number from the logical address (pageno) and returns
     * the frame number for that page if in the page table or added frame
     * to physical memory if page fault. Returns -1 if unable to get frame
     * number.
     */

    int page_fault = 0; /* Flag for whether we encounter page fault or not */
    
    if (page_table[pageno] == -1) { /* Page fault */
	/* Load BACKING_STORE.bin */
	FILE* b_store = fopen("BACKING_STORE.bin", "r");
        if (b_store == NULL) {
	    perror("BACKING_STORE.bin");
	    return -1;
        }

	/* Jump to desired page */
        if (fseek(b_store, pageno * PAGE_SIZE, SEEK_SET)) {
	    fprintf(stderr, "Error: Unable to get page from"
			    "BACKING_STORE.bin\n");
	    return -1;
        }

	/* Get page */
	if (fread(physical_mem[frame_counter], sizeof(char), PAGE_SIZE,
		  b_store) != PAGE_SIZE) {
	    fprintf(stderr, "Error: Unable to read page from"
			    "BACKING_STORE.bin\n");
	    return -1;
	}

        fclose(b_store);
        page_fault = 1;
    }

    if (page_fault) {
        page_table[pageno] = frame_counter; /* Get frame number */
        return frame_counter++;
    }
    else {
	return page_table[pageno];
    }
}


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

    /* Extract page number and page offset from logical address */
    int page_nbr = new_addr16 >> SHIFT_BYTE;
    int page_offs = new_addr16 & LOW_BYTE_MASK;

    if ((p_addr = from_TLB(page_nbr)) == -1) {
        /* TLB miss */
	if ((p_addr = from_PageTable(page_nbr)) == -1) { /* VMM error */
	    fprintf(stderr, "Unable to translate logical address to physical"
			    " address\n");
	    return -1;
	}
	
        update_TLB(page_nbr, p_addr); /* Add page and frame #'s to TLB */
    }

    /* Return physical address */
    p_addr = p_addr << SHIFT_BYTE;
    p_addr |= page_offs;
    	
    return p_addr;
}


signed char getValueAt(int physical_addr) {
    /* Given a 16-bit physical address (physical_addr), looks into physical
     * memory at the given frame and offset and returns the value at that
     * address.
     */

    int frame_nbr = physical_addr >> SHIFT_BYTE;
    int frame_offs = physical_addr & LOW_BYTE_MASK;

    return physical_mem[frame_nbr][frame_offs];
}
