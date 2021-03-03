#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE '1'
#define FALSE '0'

typedef struct {
    char valido;
    char sujo;
} page_table_entry;

page_table_entry* page_table;
unsigned mem_fill = 0;

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

void read_file(char* filename, unsigned page_offset) {
    FILE *fp;
    fp = fopen(filename, "r");

    unsigned addr;
    char rw;

    while(fscanf(fp, "%x %c", &addr, &rw) != EOF) {
        unsigned addr_page = addr >> page_offset;
        printf("Addr: %u, page: %u, op: %c\n", addr, addr_page, rw);
        printf("Page table entry at %u: ", addr_page);
        printf("VALIDO: %c, SUJO: %c\n", page_table[addr_page].valido, page_table[addr_page].sujo);
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
    page_table = (page_table_entry*)malloc(page_count * sizeof(page_table_entry));
    for(int i = 0; i < page_count; i++) {
        page_table[i].valido = FALSE;
        page_table[i].sujo = FALSE;
    }

    // Lendo e operando sobre o arquivo
    read_file(argv[2], page_offset);

    // Liberando memória alocada
    free(page_table);

    return 0;
}
