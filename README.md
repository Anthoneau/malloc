# malloc

malloc is one of the most fundamental functions in C, yet its internal mechanisms are often overlooked.

This project consists of a custom implementation of malloc, free, and realloc, built from scratch using mmap. It explores how memory allocators work internally: managing memory zones, splitting and merging blocks, handling fragmentation, and ensuring thread safety.

This project is part of the 42 specialization curriculum, focusing on low-level memory management and system-level concepts.

While the implementation itself is relatively straightforward, the real challenge lies in understanding, structuring, and documenting the allocator, as resources on the subject can be scarce and difficult to navigate.

---

## Table of Contents

- [Comparison](#comparison)
- [How it works](#how-it-works)
	- [General principle](#general-principle)
	- [Internal structure](#internal-structure)
	- [Memory organization](#memory-organization)
	- [Allocation (malloc)](#allocation-malloc)
	- [Fragmentation and split](#fragmentation-and-split)
	- [Freeing memory (free)](#freeing-memory-free)
	- [Memory zones](#memory-zones)
	- [Pre-allocation](#pre-allocation)
	- [Thread safety](#thread-safety)
- [Build & Usage](#build--usage)
	- [Compilation](#compilation)
	- [Usage in a project](#usage-in-a-project)
- [Author](#author)

---

## Project Structure

```text
malloc/
|
├── include/
|   └── malloc.h       # Main header
└── src/
	└── malloc.c       # This lonely file contains all the functions
```

---

## Comparison

I like to compare `malloc` to a restaurant. There are 4 important parts:

- the client  
- the waiter  
- the cook  
- the food supplier  

The client comes into the restaurant wanting something specific. They will always make their request to the waiter, who will then pass it to the cook to prepare the order.  
Skill is required, but above all, you need raw ingredients. The restaurant always gets them from its supplier.  

Where does this analogy lead us concretely?  
The client is our program that requests memory.  
The waiter and the cook are the `malloc` function, which handles the call, processes the request, and returns a memory block of the correct size.  
And finally, the supplier is the operating system. More precisely, it is `mmap`, the function that provides one or more memory pages to the user.

---

## How it works

I will not talk about virtual memory here. The only thing I will say is that the concept of a dynamic library (the generated `.so` file) is very useful.  
It allows having a version of a library without each program using it needing its own full copy in memory.  

Another notable point is that a program A can run with version 1.0 of the project while a program B can run with version 1.1 without any issues. In other words, the project can be recompiled while a program is using it.

---

### General principle

The goal of the project is to recreate the behavior of `malloc`, `free`, and `realloc` by managing memory ourselves.  
To obtain this memory, we use `mmap`, which allows us to request one or more memory pages directly from the operating system. A page is generally `4096 bytes`, but this value depends on the system. It can be retrieved using `getpagesize()` on macOS or `sysconf(_SC_PAGESIZE)` on Linux.  

The role of `malloc` is then to take this raw memory and split it into smaller blocks adapted to the user's needs.

---

### Internal structure

The memory is organized around two main concepts: `zones` and `chunks`.

A `chunk` represents an allocation. It contains information about the memory block, including its `size` (aligned), the `real_size` (requested by the program), a `used` flag (`used` or `free`), as well as pointers to the previous and next `chunk` in order to form a doubly linked list.

```c
typedef struct s_chunk
{
	int				used;
	size_t			size;
	size_t			real_size;
	struct s_chunk	*next;
	struct s_chunk	*prev;
}	t_chunk;
```

The memory actually returned to the program corresponds to the space located just after the `t_chunk` structure. Concretely, we return `(void *)(chunk + 1)`.

```text
Memory address:

0x1000
  │
  ▼
┌───────────────┬──────────────────────┐
│   t_chunk     │      user data       │
└───────────────┴──────────────────────┘
				▲
				│
		pointer returned by malloc
```

`chunks` are grouped into `zones`. A `zone` contains multiple allocations of the same type and stores global information such as its `size`, its `size available`, its `number of chunks`, and the list of its `chunks`.

```c
typedef struct s_zone
{
	size_t			size;
	size_t			size_available;
	size_t			n_of_chunks;
	t_chunk			*chunk;
	struct s_zone	*next;
	struct s_zone	*prev;
}	t_zone;
```

All `zones` are themselves accessible through a global structure that separates the different allocation categories.

```c
typedef struct s_alloc
{
	t_zone	*tiny;
	t_zone	*small;
	t_zone	*large;
}	t_alloc;
```

---

### Memory organization

Each `zone` corresponds to a memory block obtained with `mmap`. Inside this block, both metadata and user data are stored. Memory is therefore organized contiguously.

```text
┌───────────────────────────────────────────────────────────────┐
│                           ZONE                                │
├─────────┬──────────────┬──────────────┬──────────────┬────────┤
│ t_zone  │  t_chunk     │   data       │  t_chunk     │ data   │
│         │ (metadata)   │ (user data)  │ (metadata)   │        │
└─────────┴──────────────┴──────────────┴──────────────┴────────┘
```

This means that the `zone`, the `chunks`, and the `data` are all stored within the same memory page.

---

### Allocation (malloc)

When a user calls `malloc`, the first step is to align the requested size. `Alignment` ensures compliance with processor constraints and avoids undefined behavior. At least, that was true 25–30 years ago… some processors would crash if alignment was not respected.

It was also faster for the CPU’s MMU (memory management unit), but today the difference is negligible.  
To guarantee alignment, we use a macro that always rounds the requested size up.

```c
#define ALIGN(size) (((size) + 15) & ~15)
```

For example, a request of 17 bytes will be transformed into an actual allocation of 32 bytes.

Once the size is aligned, we determine in which `zone` this allocation should go. Depending on the size, it will be categorized as `TINY`, `SMALL`, or `LARGE`.

If a suitable `zone` already exists, we iterate through its `chunks` to find a free block large enough. When a block is found, it can be split to match the requested size exactly. If no `zone` exists yet for this type of allocation, a new one is created using `mmap`, and its first `chunk` is initialized.

---

### Fragmentation and split

In my implementation, when a `zone` is newly created, it contains two `chunks`. One corresponds to the program’s request, and the other represents all remaining available memory. When an allocation is made, this free `chunk` is split into two parts: one used part and one free part.

For example, a free `zone` of 4096 bytes becomes a used `chunk` of size 1 and a free `chunk` of size 4095 after a first `malloc(1)`.

```text
┌───────────────┐    ┌───────────────┐
│   USED        │◄──►│   FREE        │
│ size: 1       │    │ size: 4095    │
│ used: 1       │    │ used: 0       │
└───────────────┘    └───────────────┘
```

If another `malloc` is performed, for example `malloc(100)`, we reuse the existing free `chunk` instead of requesting memory again from the system, which avoids costly `mmap` calls.

```text
After malloc(100):

┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│   USED        │◄──►│   USED        │◄──►│   FREE        │
│ size: 1       │    │ size: 100     │    │ size: 3995    │
│ used: 1       │    │ used: 1       │    │ used: 0       │
└───────────────┘    └───────────────┘    └───────────────┘
```

---

### Freeing memory (free)

The `free` function does not immediately return memory to the system. It first marks the `chunk` as free by updating its flag. Then, it checks neighboring `chunks` to see if they are also free. If so, it merges the blocks to reduce fragmentation.

This mechanism helps rebuild larger free blocks and optimize future allocations. Only when an entire `zone` becomes unused do we call `munmap` to return it to the system.

Example with 2 free blocks and 1 used block:

```text
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│   FREE        │◄──►│   USED        │◄──►│   FREE        │
│ size: 1       │    │ size: 100     │    │ size: 3995    │
│ used: 0       │    │ used: 1       │    │ used: 0       │
└───────────────┘    └───────────────┘    └───────────────┘
```

The program calls `free` on the block of size 100:

```text
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│   FREE        │◄──►│   FREE        │◄──►│   FREE        │
│ size: 1       │    │ size: 100     │    │ size: 3995    │
│ used: 0       │    │ used: 0       │    │ used: 0       │
└───────────────┘    └───────────────┘    └───────────────┘
```

Instead of leaving everything as is, `free` will check the neighbors of the 100 block and merge them.

```text
┌───────────────┐
│   FREE        │
│ size: 4096    │
│ used: 0       │
└───────────────┘
```

Once done, `free` will check the number of remaining chunks in the zone, see that only one remains, and call `munmap`.

---

### Memory zones

The project requires three types of `zones`. The `TINY` and `SMALL` zones are preallocated and must contain at least 100 allocations. Their size is calculated based on the size of a `chunk` and the maximum allocation size in that zone.

The formula used is:

```text
zone_size = 100 * (sizeof(t_chunk) + n);
```

where `n` corresponds to the maximum chunk size for the given `zone`.

`LARGE` zones work differently. Each large allocation triggers a direct call to `mmap`, and each `free` calls `munmap`. These allocations are therefore not pooled and technically do not suffer from fragmentation. This is only true if the requested size is a multiple of the page size, otherwise the `zone` will necessarily be fragmented.

---

### Pre-allocation

We never request the exact size directly from `mmap`. The system always returns a full page, and that page is then reused for subsequent allocations. This reduces system calls and improves overall performance.

---

### Thread safety

The project requires the use of two global variables. The first manages allocations, while the second ensures safety in a multithreaded environment.

`malloc` must be thread-safe, which implies using a synchronization mechanism such as a `mutex` to avoid dangerous concurrent access.

A simple global `mutex` is enough to prevent concurrency issues, so that is what I implemented.

```c
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
```

At each call to `malloc`, `free`, or `realloc`, the mutex is locked at the beginning and unlocked before returning.  
Note: my implementation of thread safety is simplistic.

---

## Build & Usage

### Compilation

```bash
make
```

This will generate the dynamic libraries:

```bash
libft_malloc_$HOSTTYPE.so
```

### Usage in a project

1. Include the header:

```c
#include "malloc.h"
```

2. Compile your project with the `malloc folder`:

```bash
# Replace malloc with the path to the malloc project
cc main.c -L./malloc -lft_malloc
```

3. Run your program with a special environment variable:

```bash
LD_LIBRARY_PATH=./malloc ./your_program
```

> *This will override the system malloc for the current process.*

---

## Author

**Anthony Goldberg** *agoldber*

42 student – core curriculum completed
