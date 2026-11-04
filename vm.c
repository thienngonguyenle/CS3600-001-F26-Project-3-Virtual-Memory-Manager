/*
 * CS3600 - Virtual Memory Manager 
 * Physical memory: 64 frames; TLB: 16 entries
 * TLB replacement: FIFO; physical page replacement: LRU
 *
 * Team Name:
 *
 * Complete the TODO functions. This starter intentionally does not
 * implement address translation or replacement algorithms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define PAGE_SIZE 256
#define NUM_PAGES 256
#define NUM_FRAMES 64
#define TLB_SIZE 16
#define PHYSICAL_MEMORY_SIZE (NUM_FRAMES * PAGE_SIZE)

typedef struct {
    int frame_number;
    int valid;
} PageTableEntry;

typedef struct {
    int page_number;
    int frame_number;
    int valid;
    unsigned long long insertion_order; /* For FIFO among valid entries */
} TLBEntry;

static PageTableEntry page_table[NUM_PAGES];
static TLBEntry tlb[TLB_SIZE];
static int8_t physical_memory[PHYSICAL_MEMORY_SIZE];
static int frame_to_page[NUM_FRAMES]; /* -1 means unused */

/* LRU: update last_access[page] on EVERY reference, even a TLB hit. */
static unsigned long long last_access[NUM_PAGES];
static unsigned long long access_clock = 0;

/* TLB FIFO: insertion order, not most recent lookup. */
static unsigned long long tlb_insertion_clock = 0;
static int next_free_frame = 0;

static unsigned long long total_addresses = 0;
static unsigned long long page_faults = 0;
static unsigned long long tlb_hits = 0;
static FILE *backing_store = NULL;

static void initialize_memory(void);
static int get_page_number(int logical_address);
static int get_offset(int logical_address);
static int search_tlb(int page_number);
static void update_tlb(int page_number, int frame_number);
static void invalidate_tlb(int page_number);
static int search_page_table(int page_number);
static void load_page(int page_number, int frame_number);
static void update_lru(int page_number);
static int lru_page_replacement(void);
static int handle_page_fault(int page_number);
static int translate_address(int logical_address);
static void print_statistics(void);

static void initialize_memory(void)
{
    /* TODO: Initialize all page-table entries, TLB entries,
     * frame_to_page, last_access, clocks, and counters.
     */
}

static int get_page_number(int logical_address)
{
    /* TODO: Extract the upper eight bits. */
    (void)logical_address;
    return -1;
}

static int get_offset(int logical_address)
{
    /* TODO: Extract the lower eight bits. */
    (void)logical_address;
    return -1;
}

static int search_tlb(int page_number)
{
    /* TODO: Return the frame on a valid hit; otherwise -1.
     * Do NOT change FIFO insertion order on a hit.
     */
    (void)page_number;
    return -1;
}

static void update_tlb(int page_number, int frame_number)
{
    /* TODO: Avoid duplicate valid entries. Reuse invalid slots.
     * If full, replace the valid entry with the oldest
     * insertion_order. Update insertion order only on insertion.
     */
    (void)page_number;
    (void)frame_number;
}

static void invalidate_tlb(int page_number)
{
    /* TODO: Invalidate ALL entries mapping the evicted page. */
    (void)page_number;
}

static int search_page_table(int page_number)
{
    /* TODO: Return resident frame, or -1 if page is absent. */
    (void)page_number;
    return -1;
}

static void load_page(int page_number, int frame_number)
{
    /* TODO: Seek to page_number * PAGE_SIZE in backing_store,
     * then read exactly PAGE_SIZE bytes into physical_memory
     * starting at frame_number * PAGE_SIZE. Check for errors.
     */
    (void)page_number;
    (void)frame_number;
}

static void update_lru(int page_number)
{
    /* TODO: Increment access_clock and record this page's
     * latest reference in last_access[page_number].
     * Called once for EVERY logical address, including TLB hits.
     */
    (void)page_number;
}

static int lru_page_replacement(void)
{
    /* TODO: Only called when all 64 frames are occupied.
     * 1. Find resident page with smallest last_access value.
     * 2. Invalidate its page-table entry and all TLB mappings.
     * 3. Return its frame for reuse.
     * Do not choose the oldest-loaded page (that would be FIFO).
     */
    return -1;
}

static int handle_page_fault(int page_number)
{
    /* TODO:
     * 1. Increment page_faults.
     * 2. Use next_free_frame while free frames remain;
     *    otherwise call lru_page_replacement().
     * 3. Load the page from backing_store.
     * 4. Update page_table and frame_to_page.
     * 5. Return selected frame number.
     */
    (void)page_number;
    return -1;
}

static int translate_address(int logical_address)
{
    /* TODO:
     * 1. Extract page number and offset.
     * 2. Search TLB. Count a hit only for a valid TLB match.
     * 3. On a miss, check page table, then handle page fault
     *    if absent. Insert resolved mapping into TLB.
     * 4. Update LRU ONCE for this reference, regardless of
     *    whether the translation was a TLB hit or page fault.
     * 5. Compute physical address, read signed 8-bit value.
     * 6. Print exactly:
     *    Logical Address: N, Physical Address: N, Value: N
     * 7. Return physical address.
     */
    (void)logical_address;
    return -1;
}

static void print_statistics(void)
{
    /* TODO: Print total_addresses, page_faults, page-fault
     * rate, tlb_hits, and TLB-hit rate. Use two decimals for
     * percentages and handle zero addresses safely.
     */
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s BACKING_STORE.bin addresses.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    backing_store = fopen(argv[1], "rb");
    if (backing_store == NULL) {
        perror("Error opening backing store");
        return EXIT_FAILURE;
    }

    FILE *address_file = fopen(argv[2], "r");
    if (address_file == NULL) {
        perror("Error opening address file");
        fclose(backing_store);
        return EXIT_FAILURE;
    }

    initialize_memory();
    int logical_address;
    while (fscanf(address_file, "%d", &logical_address) == 1) {
        if (logical_address < 0 || logical_address > 65535) {
            fprintf(stderr, "Invalid logical address: %d\n", logical_address);
            fclose(address_file);
            fclose(backing_store);
            return EXIT_FAILURE;
        }
        total_addresses++;
        translate_address(logical_address);
    }

    if (ferror(address_file)) {
        perror("Error reading address file");
        fclose(address_file);
        fclose(backing_store);
        return EXIT_FAILURE;
    }

    print_statistics();
    fclose(address_file);
    fclose(backing_store);
    return EXIT_SUCCESS;
}
