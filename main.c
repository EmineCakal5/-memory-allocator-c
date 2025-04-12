#include <stdio.h>
#include "mymalloc.h"

int main() {
    printf("=== MEMORY ALLOCATOR TEST ===\n");

    void *p1 = mymalloc(24);
    void *p2 = mymalloc(40);
    void *p3 = mymalloc(48);  // 3 blok (48 byte)

    printf("Adresler:\n");
    printf("p1: %p\n", p1);
    printf("p2: %p\n", p2);
    printf("p3: %p\n", p3);

    printf("\n Mevcut heap durumu:\n");
    printheap();

    printf("\n p2 ve p3 serbest bırakılıyor...\n");
    myfree(p2);
    myfree(p3);

    printf("\n Heap durumu (birleşme gözlenmeli):\n");
    printheap();

    printf("\n Son olarak p1 de serbest bırakılıyor...\n");
    myfree(p1);

    printf("\nTüm bloklar birleşmeli:\n");
    printheap();

    return 0;
}
