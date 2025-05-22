#include "Memory.h"
#include <stdlib.h>

memory_t* mymemory_init(size_t size){
    
    // Ponteiro memory aponta para a estrutura memory_t, e em seguida reserva uma quantidade X de bytes na memória heap (com malloc). O endereço dessa área alocada é armazenado no ponteiro memory.
    memory_t *memory = (memory_t*) malloc(sizeof(memory_t));
    
    //Se a alocação falhar retornamos nulo.
    if(memory == NULL){
        return NULL;
    }

      // Fazemos uma nova alocação também no heap para criar o pool, que é a área real de memória onde os dados do usuário serão armazenados.
    memory->pool = malloc(size);
    
    //Se não tiver espaço na memória para essa reserva, ent libera tudo e retorna null
    if (memory->pool == NULL) {
        free(memory);
        return NULL;
    }

    // Por fim, inicializamos os campos da struct memory

    memory->total_size = size;   // total_size, que recebe o valor de size, representando o tamanho total do pool;
    memory->head = NULL;         // head, que recebe NULL, indicando que nenhum bloco foi alocado ainda

    // 4. Retorna o ponteiro para a estrutura completa
    return memory;
}

void* mymemory_alloc(memory_t *memory, size_t size) {
    
    //Verifica se o gerenciador foi corretamente inicializado e se o tamanho da alocação é válido. Caso contrário, retorna NULL.
    if (memory == NULL || size == 0) return NULL;

    // current começa apontando para o primeiro bloco alocado (caso exista), e prev começa como NULL porque ainda não percorremos nada na lista.
    allocation_t *current = memory->head;
    allocation_t *prev = NULL;

    // ponteiro que marca o início da região que estamos avaliando no pool e o ponteiro que acompanha byte a byte pelo pool, procurando um espaço livre
    char *pool_start = (char *) memory->pool;
    char *ptr = pool_start;

    // Enquanto houver blocos já alocados (current != NULL), calculamos o espaço livre (gap) entre ptr (posição onde poderíamos alocar) e o início do bloco atual (current->start). Se esse espaço for suficiente (gap >= size), então podemos alocar ali e saímos do loop. Se não for, movemos ptr para o fim do bloco atual e seguimos para o próximo, atualizando prev e current."
    while (current != NULL) {
        size_t gap = (char *)current->start - ptr;

        if (gap >= size) {
            break;
        }

        ptr = (char *)current->start + current->size;
        prev = current;
        current = current->next;
    }

    // Verifica se o espaço a partir de ptr tem size bytes disponíveis sem ultrapassar o limite do pool. Se ultrapassar, não há como alocar retornamos NULL.
    if ((ptr + size) > (pool_start + memory->total_size)) {
        return NULL; // não há espaço suficiente
    }

    // cria nova alocação se falhar retorna nulo
    allocation_t *new_block = (allocation_t *) malloc(sizeof(allocation_t));
    if (new_block == NULL) return NULL;


    //Atualizamos os valores desse novo bloco.
    new_block->start = ptr;
    new_block->size = size;
    new_block->next = current;

    //Se não há bloco anterior (prev == NULL), esse será o primeiro da lista. Caso contrário, inserimos o novo bloco depois de prev, preservando a ordem.
    if (prev == NULL) {
        memory->head = new_block;
    } else {
        prev->next = new_block;
    }

    return ptr; // devolve o ponteiro para o bloco alocado
}


//Não retorna nada e recebe como parametro um ponteiro para a memória do sistema e um ponteiro para o bloco que será liberado
//Simula o free()
//Percorre a lista de alocações, encontra o bloco que começa com ptr e o remove da lista
void mymemory_free(memory_t *memory, void *ptr) {
    if (memory == NULL || ptr == NULL) return;

    allocation_t *current = memory->head;
    allocation_t *prev = NULL;

    // percorre a lista procurando o bloco com start == ptr
    while (current != NULL) {
        if (current->start == ptr) {
            // achou o bloco, agora remove da lista
            if (prev == NULL) {
                // o bloco é o primeiro da lista
                memory->head = current->next;
            } else {
                // o bloco está no meio ou fim
                prev->next = current->next;
            }

            // libera a struct allocation_t (mas não mexe no pool!)
            free(current);
            return;
        }

        // avança na lista
        prev = current;
        current = current->next;
    }

    // se chegou aqui, não achou o bloco — não faz nada
}

//Mostra os blocos alocados
//Mostra todos os blocos em ordem: posição start e tamanho
void mymemory_display(memory_t *memory) {
    if (memory == NULL) {
        printf("Memória não inicializada.\n");
        return;
    }

    allocation_t *current = memory->head;

    if (current == NULL) {
        printf("Nenhum bloco alocado.\n");
        return;
    }

    printf("Blocos alocados:\n");

    while (current != NULL) {
        printf(" - Início: %p | Tamanho: %zu bytes\n", current->start, current->size);
        current = current->next;
    }
}

//Mostra estatísticas da memória 
//Total alocado, total livre, maior espaço contíguo livre, fragmentação
#include <stdio.h>

void mymemory_stats(memory_t *memory) {
    if (memory == NULL) {
        printf("Memória não inicializada.\n");
        return;
    }

    size_t total_allocated = 0;
    size_t total_free = 0;
    size_t largest_free_block = 0;
    int num_allocations = 0;
    int num_free_fragments = 0;

    char *pool_start = (char *) memory->pool;
    char *pool_end = pool_start + memory->total_size;

    allocation_t *current = memory->head;
    char *ptr = pool_start;

    while (current != NULL) {
        size_t gap = (char *)current->start - ptr;

        if (gap > 0) {
            total_free += gap;
            num_free_fragments++;
            if (gap > largest_free_block)
                largest_free_block = gap;
        }

        total_allocated += current->size;
        ptr = (char *)current->start + current->size;
        current = current->next;
        num_allocations++;
    }

    // espaço livre depois do último bloco até o fim do pool
    if (ptr < pool_end) {
        size_t final_gap = pool_end - ptr;
        total_free += final_gap;
        num_free_fragments++;
        if (final_gap > largest_free_block)
            largest_free_block = final_gap;
    }

    printf("=== Estatísticas da Memória ===\n");
    printf("Total de alocações: %d\n", num_allocations);
    printf("Total alocado: %zu bytes\n", total_allocated);
    printf("Total livre: %zu bytes\n", total_free);
    printf("Maior bloco livre contíguo: %zu bytes\n", largest_free_block);
    printf("Número de fragmentos livres: %d\n", num_free_fragments);
}

//liberar tudo ao final
//Da free no pool e em todas as structs allocation_t
void mymemory_cleanup(memory_t *memory) {
    if (memory == NULL) return;

    allocation_t *current = memory->head;
    while (current != NULL) {
        allocation_t *next = current->next;
        free(current);
        current = next;
    }

    free(memory->pool);
    free(memory);
}
