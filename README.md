# CS3600-001-F26-Project-3-Virtual-Memory-Manager

## 1. Project Overview

In this project, you will design and implement a **Virtual Memory Manager** in C that simulates how an operating system translates logical addresses into physical addresses.

Your program will implement:

- Logical-to-physical address translation
- Page table management
- Translation Lookaside Buffer (TLB)
- Demand paging
- Page fault handling
- FIFO TLB replacement
- LRU page replacement
- Performance statistics

The virtual address space contains 256 pages, but physical memory can hold only 64 pages at a time.

Because physical memory is limited, your program must implement the **Least Recently Used (LRU)** page-replacement algorithm.

The TLB will use the **First-In, First-Out (FIFO)** replacement algorithm.

You will develop **one complete virtual memory management program** that integrates all these mechanisms.


## 2. System Specifications

Your virtual memory manager must use the following configuration:

| Component | Specification |
|---|---|
| Logical address size | 16 bits |
| Virtual address space | 65,536 bytes |
| Page size | 256 bytes |
| Number of virtual pages | 256 |
| Page table size | 256 entries |
| TLB size | 16 entries |
| Physical memory size | 16,384 bytes |
| Number of physical frames | 64 |
| Frame size | 256 bytes |
| TLB replacement algorithm | FIFO |
| Page replacement algorithm | LRU |
| Backing store | `BACKING_STORE.bin` |
| Programming language | C |

Initially:

- Physical memory is empty.
- All page-table entries are invalid.
- All TLB entries are invalid.
- No physical frames are allocated.
- All performance counters are zero.

---

## 3. Starter Files Provided

The following files are provided to help you complete the project.

| File | Description |
|---|---|
| `vm.c` | Starter C source code containing the program structure, data structures, function declarations, and TODO sections |
| `BACKING_STORE.bin` | Binary file containing the data for all 256 virtual pages |
| `addresses.txt` | Input file containing 1,000 logical addresses |
| `large_address.txt` | Larger input file containing 10,000 logical addresses |
| `correct.txt` | Reference signed-byte values for `addresses.txt` |
| `large_correct.txt` | Reference signed-byte values for `large_address.txt` |
| `README.md` | Project description, requirements, and instructions |

### Important Notes About the Starter Code

The provided `vm.c` is **incomplete**.

It includes the basic structure of the program, but you must implement the virtual memory management algorithms.

You are responsible for completing the functions marked with `TODO`.

You may:

- Add helper functions.
- Add new data structures.
- Modify existing functions.
- Introduce additional variables.
- Reorganize the code when necessary.

However, your final program must satisfy all project requirements.

You must not hardcode expected output values or use the reference files as lookup tables.

---

## 4. Project Requirements

### 4.1 Logical Address Translation

Your program must read logical addresses from the input file and translate them into physical addresses.

Each logical address is a 16-bit integer consisting of:

- **Page number:** Upper 8 bits
- **Offset:** Lower 8 bits

You must extract the page number and offset using bitwise operations.

For example:

```text
Logical Address: 32769

Binary Representation:
10000000 00000001

Page Number: 128
Offset: 1
```

The physical address is calculated using:

```text
Physical Address = (Frame Number * PAGE_SIZE) + Offset
```

Since the page size is 256 bytes:

```text
Physical Address = (Frame Number * 256) + Offset
```

The physical frame number depends on the current state of physical memory.

---

### 4.2 Translation Lookaside Buffer (TLB)

Implement a TLB containing **16 entries**.

Each TLB entry must store at least:

- Page number
- Frame number
- Valid bit

For every logical address:

1. Search the TLB for the requested page number.
2. If a valid matching entry exists, retrieve its frame number.
3. Increment the TLB-hit counter.
4. If the page is not found in the TLB, search the page table.
5. After resolving a TLB miss, insert the valid page-to-frame mapping into the TLB.

#### FIFO TLB Replacement

When the TLB is full, your program must use the **First-In, First-Out (FIFO)** replacement algorithm.

The entry that has been in the TLB the longest must be replaced.

FIFO replacement is based on insertion order, not how recently an entry was accessed.

A TLB hit must not change the FIFO replacement order.

If an entry is invalidated, its slot may be reused. Your implementation must preserve FIFO ordering among the remaining valid entries.

Your implementation must avoid duplicate valid mappings for the same page.

**Important:** Replacing a TLB entry does not remove its page from physical memory.

---

### 4.3 Page Table

Implement a page table containing **256 entries**.

Each page-table entry must contain at least:

- Frame number
- Valid bit

The page table records whether a virtual page is currently loaded into physical memory.

When a page is present, the page table identifies the physical frame containing that page.

When a page is not present, a page fault occurs.

A TLB miss does not necessarily mean a page fault.

If the page is already loaded in physical memory, the frame number can be obtained directly from the page table.

---

### 4.4 Demand Paging and Page Fault Handling

Initially, physical memory is empty.

Pages must be loaded into physical memory only when they are requested.

When a page fault occurs, your program must:

1. Increment the page-fault counter.
2. Locate the requested page in `BACKING_STORE.bin`.
3. Determine which physical frame will hold the page.
4. If an unused frame is available, allocate that frame.
5. Otherwise, invoke the LRU page-replacement algorithm.
6. Load the requested page into the selected frame.
7. Update the page table.
8. Update the frame-to-page mapping.
9. Ensure the TLB contains the correct mapping before translation completes.

The location of a page in the backing store is calculated as:

```text
Backing Store Offset = Page Number * PAGE_SIZE
```

For example:

```text
Page Number: 10
Page Size: 256 bytes

Backing Store Offset = 10 * 256 = 2560
```

You may use the following C functions:

```c
fseek()
fread()
```

Each page load must read exactly 256 bytes.

Your program must detect and report file-reading errors.

---

### 4.5 LRU Page Replacement

Physical memory contains only **64 frames**.

When all frames are occupied and a page fault occurs, your program must use the **Least Recently Used (LRU)** page-replacement algorithm.

LRU replaces the page that has not been accessed for the longest period of time.

#### LRU Requirements

Your program must:

1. Track the most recent access to every resident page.
2. Update the access information whenever a page is referenced.
3. Include both TLB hits and TLB misses when updating access information.
4. Identify the least recently used resident page when replacement is necessary.
5. Evict that page from physical memory.
6. Invalidate the evicted page's page-table entry.
7. Invalidate all TLB entries referencing the evicted page.
8. Load the requested page into the selected frame.
9. Update all affected mappings and LRU information.

#### Example

Suppose physical memory contains four frames for illustration purposes:

```text
Frame 0: Page 5
Frame 1: Page 8
Frame 2: Page 2
Frame 3: Page 9
```

Assume the pages were most recently accessed in this order:

```text
Page 8 -> Page 2 -> Page 9 -> Page 5
```

Page 8 is the least recently used page.

If a new page must be loaded, page 8 should be evicted.

The new page will occupy the frame previously assigned to page 8.

#### Suggested LRU Implementation

You may implement LRU using:

- Access timestamps
- Logical access counters
- An ordered list
- Another correct LRU tracking mechanism

For example, you may maintain a global counter that increases for every logical address processed.

Each resident page records the counter value of its most recent access.

When replacement is required, select the resident page with the smallest last-access value.

**Important:** LRU is based on page accesses, not page-loading order.

A TLB hit must also update the page's LRU information.

You must implement true LRU behavior. FIFO page replacement will not satisfy this requirement.

---

### 4.6 Maintaining Consistency

Your virtual memory manager must keep the following components consistent:

- Page table
- TLB
- Physical memory
- Frame-to-page mapping
- LRU tracking information

When a page is evicted:

1. Its page-table entry must become invalid.
2. Any corresponding TLB entries must become invalid.
3. Its physical frame becomes available for the incoming page.
4. The incoming page must receive the correct mappings.
5. Its LRU information must reflect the current access.

A valid TLB entry must never reference a page that is no longer resident in physical memory.

---

### 4.7 Retrieving Memory Values

After translating a logical address, retrieve the byte stored at the corresponding physical address.

Interpret the byte as a **signed 8-bit integer**.

The valid range is:

```text
-128 to 127
```

For example:

```text
Unsigned Byte: 255
Signed Byte: -1
```

The retrieved value must match the byte stored at the corresponding logical address in the backing store.

---

## 5. Program Output

For each logical address, your program must display:

- Logical address
- Physical address
- Signed byte value

Use the following format:

```text
Logical Address: 32769, Physical Address: 1, Value: -42
```

The physical address and value above are illustrative.

The actual physical address depends on the current page-to-frame mapping, and the value depends on the backing-store contents.

### 5.1 Performance Statistics

After processing all logical addresses, your program must display:

1. Total number of translated addresses
2. Total number of page faults
3. Page-fault rate
4. Total number of TLB hits
5. TLB-hit rate

Use the following formulas:

```text
Page Fault Rate = (Page Faults / Total Addresses) * 100%

TLB Hit Rate = (TLB Hits / Total Addresses) * 100%
```

Both rates must be displayed as percentages with two decimal places.

Example output format:

```text
========== Performance Statistics ==========

Total Addresses: 1000
Page Faults: ...
Page Fault Rate: ...%
TLB Hits: ...
TLB Hit Rate: ...%

============================================
```

The statistics above are placeholders, not expected results.

Your program must calculate the actual statistics from its execution.

A TLB hit occurs only when a valid translation is found directly in the TLB.

A page fault occurs only when the requested page is not currently loaded in physical memory.

---

## 6. Recommended Program Structure

You are encouraged to organize your implementation into functions.

The following structure is suggested:

```c
/* Initialization */
void initialize_memory(void);

/* Logical address extraction */
int get_page_number(int logical_address);
int get_offset(int logical_address);

/* TLB management */
int search_tlb(int page_number);
void update_tlb(int page_number, int frame_number);
void invalidate_tlb(int page_number);

/* Page table management */
int search_page_table(int page_number);

/* Demand paging */
void load_page(int page_number, int frame_number);
int handle_page_fault(int page_number);

/* LRU page replacement */
void update_lru(int page_number);
int lru_page_replacement(void);

/* Address translation */
int translate_address(int logical_address);

/* Statistics */
void print_statistics(void);
```

You may change the function names, parameters, or return types as needed.

The important requirement is that your implementation correctly integrates all virtual memory management mechanisms.

---

## 7. Recommended Implementation Steps

You are encouraged to implement and test the project incrementally.

### Step 1: Initialize the System

Implement:

```c
initialize_memory()
```

Initialize the page table, TLB, frame mappings, LRU tracking information, and statistics.

### Step 2: Extract Address Components

Implement:

```c
get_page_number()
get_offset()
```

Verify that page numbers and offsets are extracted correctly.

### Step 3: Implement the Page Table

Implement:

```c
search_page_table()
```

Verify that resident pages can be located correctly.

### Step 4: Implement Demand Paging

Implement:

```c
load_page()
handle_page_fault()
```

Verify that pages can be loaded from the backing store into unused physical frames.

### Step 5: Implement LRU Page Replacement

Implement:

```c
update_lru()
lru_page_replacement()
```

Verify that the least recently used page is selected when physical memory is full.

### Step 6: Implement TLB Management

Implement:

```c
search_tlb()
update_tlb()
invalidate_tlb()
```

Verify that TLB hits, misses, FIFO replacement, and invalidation work correctly.

### Step 7: Integrate Address Translation

Implement:

```c
translate_address()
```

Connect the TLB, page table, demand paging, LRU replacement, and physical memory access.

### Step 8: Implement Statistics

Implement:

```c
print_statistics()
```

Calculate and display page-fault and TLB-hit statistics.

### Step 9: Test the Complete Program

Run your implementation using both provided input files.

Verify memory values, replacement behavior, and statistics.

---

## 8. Compilation and Execution

Your program must compile using GCC.

### Compile

```bash
gcc vm.c -o vm

```
You can also compile your program using:
```bash
gcc -Wall -Wextra -std=c11 vm.c -o vm

```
This command enables additional compiler warnings to help identify potential issues in your code and ensures that your program follows the C11 language standard.

If your implementation uses multiple source files, include them in the compilation command.

### Run with Standard Input

```bash
./vm BACKING_STORE.bin addresses.txt
```

### Run with Large Input

```bash
./vm BACKING_STORE.bin large_address.txt
```

### Save Program Output

```bash
./vm BACKING_STORE.bin addresses.txt > output.txt
```

For the larger input:

```bash
./vm BACKING_STORE.bin large_address.txt > large_output.txt
```

Your program must process logical addresses until the end of the input file.

Do not assume that every input file contains exactly 1,000 addresses.

---

## 9. Testing and Verification

You must test your implementation using both provided input files.

### Test 1: Standard Input

Run:

```bash
./vm BACKING_STORE.bin addresses.txt
```

Verify that:

- All 1,000 addresses are processed.
- The retrieved signed-byte values are correct.
- Page faults are handled correctly.
- FIFO TLB replacement works correctly.
- LRU page replacement works correctly.
- Performance statistics are calculated correctly.

### Test 2: Large Input

Run:

```bash
./vm BACKING_STORE.bin large_address.txt
```

Verify that:

- All 10,000 addresses are processed.
- All retrieved signed-byte values are correct.
- The program handles repeated page accesses.
- LRU tracking is updated correctly.
- Page-table and TLB mappings remain consistent.
- The program does not crash or access invalid memory.

### Reference Output Files

The files `correct.txt` and `large_correct.txt` contain reference signed-byte values.

You may use them to verify the values retrieved from physical memory.

However, your program also prints logical addresses, physical addresses, and performance statistics.

Therefore, you should compare the retrieved signed-byte values rather than comparing the complete output files directly.


### Additional Tests

You should also test the following situations:

1. Accessing the same page repeatedly.
2. Accessing more than 16 distinct pages to trigger TLB replacement.
3. Accessing more than 64 distinct pages to trigger page replacement.
4. Accessing a recently used page before memory becomes full.
5. Accessing a page that was previously evicted.
6. Verifying that a TLB hit updates LRU information.
7. Verifying that an evicted page no longer has a valid TLB mapping.
8. Processing an empty address file without dividing by zero.

### LRU Verification Example

Consider a simplified system with three physical frames.

The page reference sequence is:

```text
1, 2, 3, 1, 4
```

After loading pages 1, 2, and 3, all frames are occupied.

Page 1 is accessed again, making it the most recently used page.

When page 4 is requested, page 2 is the least recently used page.

Therefore, LRU must evict page 2, not page 1.

This example illustrates why LRU differs from FIFO.

---

## 10. Important Notes

1. **Use FIFO for TLB replacement.**
2. **Use LRU for physical page replacement.**
3. Physical memory must contain exactly 64 frames.
4. The page table must contain 256 entries.
5. The TLB must contain 16 entries.
6. A TLB miss does not necessarily cause a page fault.
7. Every memory access must update the relevant page's LRU information, including TLB hits.
8. Replacing a TLB entry does not remove the page from physical memory.
9. Evicting a page must invalidate its page-table and TLB mappings.
10. Pages must be loaded from `BACKING_STORE.bin` on demand.
11. Each page load must read 256 bytes.
12. Retrieved values must be interpreted as signed 8-bit integers.
13. Your program must process both provided input files.
14. Your program must calculate page-fault and TLB-hit statistics.
15. Do not hardcode expected outputs or performance statistics.

---

## 11. Academic Integrity

You are expected to understand and be able to explain your implementation.
You may consult course materials, textbooks, and other permitted learning resources, but you are responsible for understanding the algorithms and code you submit.

# 12. Submitting Your Work

When you are ready to submit your project, make sure that all of your final changes have been committed to your local Git repository.

You can check the status of your repository using:

```bash
git status
```

Make sure there are no important uncommitted changes.

Next, create a Git bundle containing your repository and its complete Git history.

Run:

```bash
git bundle create <team_name>.bundle --all
```

Replace `<team_name>` with your team's name.

For example, if your team name is Awesome, use:

```bash
git bundle create team_awesome.bundle --all
```

This creates:

```text
team_awesome.bundle
```

Use the following naming format:

```text
team_<team_name>.bundle
```

Using the correct filename format is important because it helps identify your team during grading.

You may verify your bundle before submitting it using:

```bash
git bundle verify team_<team_name>.bundle
```

Finally, submit only the `.bundle` file to the project assignment on Canvas before the deadline.
