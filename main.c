#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "const.h"
#include "paging.h"

// Número de bits do offset baseado no tamanho da página
unsigned get_page_offset(unsigned page_size) {
    unsigned s, tmp;
    tmp = page_size;
    s = 0;
    while(tmp > 1) {
        tmp = tmp >> 1;
        s++;
    }
    return s;
}

// Número de páginas baseado no offset
unsigned get_page_addr(unsigned offset) { return 32 - offset; }

// Tamanho da tabela de páginas baseado no número
unsigned get_page_count(unsigned page_addr) { return pow(2, page_addr); }

// Número de quadros baseado no tamanho do quadro e na memória disponível (em kB)
unsigned get_frame_count(unsigned frame_size, unsigned memory_size) { return memory_size / frame_size; }

void read_file(char* filename, unsigned frame_count, page_table_entry* page_table, unsigned page_offset, char* replace) {
    FILE *fp;
    fp = fopen(filename, "r");

    unsigned addr;
    char rw;

    unsigned mem_filled = 0;
    unsigned fault_count = 0;
    unsigned dirty_count = 0;

    unsigned* frame_table = (unsigned*)malloc(frame_count * sizeof(unsigned));
    for(int i = 0; i < frame_count; i++) {
        frame_table[i] = INVALID;
    }

    queue* fifo_queue = init_queue();
    long lru_timestamp = 0; 

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    while(fscanf(fp, "%x %c", &addr, &rw) != EOF) {
        // ERRO: leu linha sem comando R (read) ou W (write)
        if(rw != READ && rw != WRITE) {
            printf("ERRO: Comando inválido ou sem comando\n");
            break;
        }

        unsigned addr_page = addr >> page_offset;
        
        // Page hit
        if(page_table[addr_page].frame_addr != INVALID) {
            if(rw == WRITE) {
                page_table[addr_page].dirty = TRUE;
            }
        }
        // Page fault
        else {
            fault_count++;
            // Enquanto todos os endereços físicos não forem mapeados, não é necessário substituir.
            if(mem_filled < frame_count) {
                page_table[addr_page].frame_addr = mem_filled;
                if(rw == WRITE) {
                    page_table[addr_page].dirty = TRUE;
                }
                
                if(strcmp(replace, FIFO) == 0)
                    push_queue(fifo_queue, addr_page);


                frame_table[mem_filled] = addr_page;
                mem_filled++;
            }
            else {
                // Quando todos os endereços da memória física foram mapeados na tabela,
                // substituição começa.

                // Selecionando via algoritmo de substituição
                unsigned addr_sub;
                if(strcmp(replace, FIFO) == 0)
                    addr_sub = pop_queue(fifo_queue);
                else if(strcmp(replace, LRU) == 0)
                    addr_sub = get_lru(page_table, frame_table, frame_count);
                else if(strcmp(replace, RANDOM) == 0)
                    addr_sub = frame_table[rand() % (frame_count - 1)];

                // Substituindo
                short tmp = page_table[addr_sub].frame_addr;
                page_table[addr_sub].frame_addr = INVALID;
                if(page_table[addr_sub].dirty == TRUE) {
                    dirty_count++;
                    page_table[addr_sub].dirty = FALSE;
                }
                page_table[addr_page].frame_addr = tmp;
                if(rw == WRITE) {
                    page_table[addr_page].dirty = TRUE;
                }
                
                if(strcmp(replace, FIFO) == 0)
                    push_queue(fifo_queue, addr_page);

                frame_table[tmp] = addr_page;
            }
        }
        page_table[addr_page].timestamp = lru_timestamp++;
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double t = 1.e+6 * (double)(end.tv_sec - start.tv_sec) + 1.e-3 * (double)(end.tv_nsec - start.tv_nsec);
    t = t / 1.e+6;

    printf("Total page faults: %u\n", fault_count);
    printf("Total dirty writes: %u\n", dirty_count);
    printf("Execution time: %.5lfs\n", t);


    free(frame_table);
    delete_queue(fifo_queue);
    fclose(fp);
}

int main(int argc, char **argv) {

    time_t t;
    srand((unsigned)time(&t));

    // tp2virtual [algo.] [arquivo] [tamanho pág.] [memória]
    unsigned page_offset = get_page_offset(atoi(argv[3]) * 1024);
    unsigned page_count = get_page_count(get_page_addr(page_offset));
    unsigned frame_count = atoi(argv[4]) / atoi(argv[3]);
    printf("Page count: %u\n", page_count);
    printf("Frame count: %u\n", frame_count);

    // Inicializando tabela de páginas
    page_table_entry* page_table = (page_table_entry*)malloc(page_count * sizeof(page_table_entry));
    for(int i = 0; i < page_count; i++) {
        page_table[i].frame_addr = INVALID;
        page_table[i].dirty = FALSE;
        page_table[i].timestamp = INVALID;
    }

    // Lendo e operando sobre o arquivo
    read_file(argv[2], frame_count, page_table, page_offset, argv[1]);

    // Liberando memória alocada
    free(page_table);

    return 0;
}