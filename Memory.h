#ifndef MYMEMORY_H
#define MYMEMORY_H

#include <stddef.h>

// Bloco de memória que foi alocado
typedef struct allocation {
    void *start;
    size_t size;
    struct allocation *next;
} allocation_t;

// Representa todo o sistema de gerenciamento de memória
typedef struct {
    void *pool; 
    size_t total_size;
    allocation_t *head;
} memory_t;

memory_t* mymemory_init(size_t size);
void* mymemory_alloc(memory_t *memory, size_t size);
void mymemory_free(memory_t *memory, void *ptr);
void mymemory_display(memory_t *memory);
void mymemory_stats(memory_t *memory);
void mymemory_cleanup(memory_t *memory);

#endif /* MYMEMORY_H */