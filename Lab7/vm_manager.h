#ifndef VM_MANAGER
#define VM_MANAGER

/* MACROS */
/* Maximums (Just random numbers btw) */
#define MAX_INPUT_LINE_LEN 80
#define MAX_OUTPUT_LINE_LEN 200

/* Lab 7 Specifications */
#define NUM_PAGES 256 /* 2^8 page entries in page table */
#define PAGE_SIZE 256 /* 2^8 bytes in a page */
#define NUM_FRAMES 256 /* 256 frames */
#define FRAME_SIZE 256 /* 2^8 bytes in a frame */
#define NUM_TLB_ENTRIES 16 /* 16 entries in TLB */
#define NUM_LOG_ADDRS 1000 /* 1000 logical addresses */
#define MEMORY_SPACE 65536 /* Physical memory is 65536 bytes */

/* Other */
#define BOTH_BYTES_MASK 0xFFFF /* Get 16-bit from 32-bit */
#define LOW_BYTE_MASK 0xFF /* Get 8-bit from 16-bit */
#define SHIFT_BYTE 8 /* To shift a value a byte */


/* FUNCTIONS */
void init_Manager(void);
int from_TLB(int pageno);
void update_TLB(int pageno, int frameno);
int from_PageTable(int pageno);
int get_PAddress(char* addr);
signed char getValueAt(int physical_addr);

#endif /* VM_MANAGER_H */
