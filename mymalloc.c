#include "mymalloc.h"
#include <unistd.h>  // sbrk
#include <stddef.h>  // NULL, offsetof

// Strateji ve liste tipi (başlangıç ayarları)
Strategy strategy = FIRST_FIT;
ListType listtype = ADDR_ORDERED_LIST;

// Global değişkenler
Block *free_list = NULL;
Block *heap_start = NULL;
Block *heap_end = NULL;
Block *last_freed = NULL;

// Bellek boyutunu 16 baytlık bloklara yuvarlar
uint64_t numberof16blocks(size_t size_inbytes) {
    return (size_inbytes + 15) / 16;
}

// Bloku ikiye böler; sol taraf kullanıcıya ayrılır, sağ taraf free olarak kalır
Block *split_block(Block *b, size_t requested_blocks) {
    size_t remaining = b->info.size - requested_blocks - (sizeof(Block) / 16);

    if (remaining < 1) return b; // Bölünecek kadar büyük değilse bölme

    // Yeni blok adresi: eski bloğun data[] alanından sonra başlar
    Block *newblock = (Block *) (b->data + requested_blocks * 16);
    newblock->info.size = remaining;
    newblock->info.isfree = 1;

    // Yeni bloğu free_list'e bağla
    newblock->next = b->next;
    newblock->prev = b->prev;

    if (b == free_list) {
        free_list = newblock;
    }

    // Orijinal blok güncellenir
    b->info.size = requested_blocks;
    b->info.isfree = 0;
    b->next = NULL;
    b->prev = NULL;

    return b;
}

// Bellek ayırma fonksiyonu
void *mymalloc(size_t size) {
    if (size == 0) return NULL;

    // Kaç tane 16 baytlık blok gerektiğini hesapla
    uint64_t blocks_needed = numberof16blocks(size);

    // İlk kez malloc çağrıldıysa: heap'ten alan al
    if (heap_start == NULL) {
        void *heap = sbrk(HEAP_SIZE);
        if (heap == (void *) -1) return NULL; // sbrk başarısızsa

        // Yeni boş blok oluştur
        Block *b = (Block *) heap;
        b->info.size = numberof16blocks(HEAP_SIZE - sizeof(Block));
        b->info.isfree = 1;
        b->next = NULL;
        b->prev = NULL;

        heap_start = b;
        heap_end = (Block *) ((char *)heap + HEAP_SIZE);
        free_list = b;
    }

    // Serbest blokları tara (first fit)
    Block *curr = free_list;
    while (curr != NULL) {
        if (curr->info.isfree && curr->info.size >= blocks_needed) {
            // Blok uygun büyüklükteyse ve bölünecekse böl
            if (curr->info.size >= blocks_needed + (sizeof(Block) / 16) + 1) {
                split_block(curr, blocks_needed);
            }

            // Blok artık dolu
            curr->info.isfree = 0;

            // Free list'ten çıkar
            if (curr->prev) curr->prev->next = curr->next;
            else free_list = curr->next;

            if (curr->next) curr->next->prev = curr->prev;

            return curr->data; // Kullanıcıya data[] kısmını döndür
        }
        curr = curr->next;
    }

    return NULL; // Uygun blok bulunamadı
}
Block *next_block_in_addr(Block *b) {
    return (Block *)((char *)b + sizeof(Block) + (b->info.size * 16));
}

Block *prev_block_in_addr(Block *b) {
    // heap'in başındaysa geri gitme
    Block *curr = heap_start;
    Block *prev = NULL;

    while (curr && curr < b) {
        prev = curr;
        curr = next_block_in_addr(curr);
    }

    return prev;
}
Block *right_coalesce(Block *b) {
    Block *right = next_block_in_addr(b);
    if ((void *)right >= (void *)heap_end) return b;
    if (right->info.isfree) {
        b->info.size += right->info.size + (sizeof(Block) / 16);
        // free_list bağlantılarını düzeltmek istersen buraya ekleyebilirsin
    }
    return b;
}

Block *left_coalesce(Block *b) {
    Block *left = prev_block_in_addr(b);
    if (left && left->info.isfree) {
        left->info.size += b->info.size + (sizeof(Block) / 16);
        // free_list bağlantılarını düzeltmek istersen buraya ekleyebilirsin
        return left;
    }
    return b;
}

// Belleği serbest bırakma fonksiyonu
void myfree(void *p) {
    if (!p) return;

    // data[] adresinden Block başlangıcına geri dön
    Block *b = (Block *)((char *)p - offsetof(Block, data));
    b->info.isfree = 1;

    b = right_coalesce(b);
    b = left_coalesce(b);

    // Free list'in başına ekle (LIFO mantığı)
    b->next = free_list;
    if (free_list) free_list->prev = b;
    b->prev = NULL;
    free_list = b;
}
#include <stdio.h> 

void printheap() {
    printf("======= HEAP DURUMU =======\n");

    Block *curr = heap_start;
    while ((void *)curr < (void *)heap_end) {
        printf("Free: %d\n", curr->info.isfree);
        printf("Size: %lu bytes\n", curr->info.size * 16); // çünkü 16 baytlık bloklar
        printf("---------------------------\n");

        // Sonraki blokun adresine git
        curr = (Block *)((char *)curr + sizeof(Block) + (curr->info.size * 16));
    }
}
