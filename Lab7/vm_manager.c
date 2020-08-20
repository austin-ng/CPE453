#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_manager.h"


static int logical_addrs[NUM_LOG_ADDRS];
static int log_addrs_len = 0;


void add_LAddress(char* addr) {
    int new_addr32 = atoi(addr);
    int new_addr16 = new_addr32 & LONG_MASK;

    if ((new_addr16 == 0) && (addr[0] == '0')) {
	fprintf(stderr, "Unable to add logical address '%s' into manager\n",
			addr);
    }
    else {
	logical_addrs[log_addrs_len++] = new_addr16;
    }
}
