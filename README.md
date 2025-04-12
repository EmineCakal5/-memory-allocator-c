# Memory Allocator in C

This project is a simple custom memory allocator implemented in C using the `sbrk()` system call. It replicates basic dynamic memory management with `mymalloc` and `myfree` functions.

## Features

- Custom malloc and free implementation
- First-Fit allocation strategy
- Block splitting and merging (coalescing)
- 16-byte memory alignment
- Heap visualization with printheap()

## Files

- `mymalloc.c` - Implementation of memory allocator
- `mymalloc.h` - Struct definitions and function declarations
- `main.c` - Test program

## Usage

```bash
gcc -o allocator main.c mymalloc.c
./allocator
```

## Author

Emine Çakal  
Computer Engineering Student  
Istanbul Medeniyet University
