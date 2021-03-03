#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE '1'
#define FALSE '0'
#define READ 'R'
#define WRITE 'W'

typedef struct {
    char valid;
    char dirty;
} page_table_entry;

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

void read_file(char* filename, unsigned frame_count, page_table_entry* page_table, unsigned page_offset) {
    FILE *fp;
    fp = fopen(filename, "r");

    unsigned addr;
    char rw;

    unsigned mem_filled = 0;
    unsigned fault_count = 0;
    unsigned dirty_count = 0;

    while(fscanf(fp, "%x %c", &addr, &rw) != EOF) {
        // ERRO: leu linha sem comando R (read) ou W (write)
        if(rw != READ && rw != WRITE) {
            printf("ERRO: Comando inválido ou sem comando\n");
            break;
        }

        unsigned addr_page = addr >> page_offset;
        printf("Addr: %u, page: %u, op: %c\n", addr, addr_page, rw);
        printf("Page table entry at %u: ", addr_page);
        printf("VALIDO: %c, SUJO: %c\n", page_table[addr_page].valid, page_table[addr_page].dirty);
        
        // Page hit
        if(page_table[addr_page].valid == TRUE) {
            if(rw == WRITE) {
                page_table[addr_page].dirty = TRUE;
            }
        }
        // Page fault
        else {
            fault_count++;
            // Enquanto todos os endereços físicos não forem mapeados, não é necessário substituir.
            if(mem_filled < frame_count) {
                page_table[addr_page].valid = TRUE;
                if(rw == WRITE) {
                    page_table[addr_page].dirty = TRUE;
                }
            }
            else {
                // Quando todos os endereços da memória física foram mapeados na tabela,
                // substituição começa.
                unsigned addr_sub = 1;// TODO: INSERT ALGO HERE
                
            }
        }

        printf("Total page faults: %u\n", fault_count);
    }

    fclose(fp);
}

int main(int argc, char **argv) {

    // tp2virtual [algo.] [arquivo] [tamanho pág.] [memória]
    unsigned page_offset = get_page_offset(atoi(argv[3]) * 1024);
    unsigned page_count = get_page_count(get_page_addr(page_offset));
    unsigned frame_count = atoi(argv[4]) / atoi(argv[3]);
    printf("Page count: %u\n", page_count);

    // Inicializando tabela de páginas
    page_table_entry* page_table = (page_table_entry*)malloc(page_count * sizeof(page_table_entry));
    for(int i = 0; i < page_count; i++) {
        page_table[i].valid = FALSE;
        page_table[i].dirty = FALSE;
    }

    // Lendo e operando sobre o arquivo
    read_file(argv[2], frame_count, page_table, page_offset);

    // Liberando memória alocada
    free(page_table);

    return 0;
}