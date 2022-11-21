#include <stdio.h>

#include "symtable.h"

#define P(str) printf("%s\n", (char*)symtable_find(t, str)->value)
#define A(nazev, value) symtable_add(t, nazev)->value = (void*)(value);

void free_string(void* p) { free((char*)p); }

char* make_string(char* c) {
    size_t len = strlen(c);
    char* ptr = malloc(len + 1);
    strcpy(ptr, c);
    return ptr;
}

void check_empty(struct symtable* t, char* string) {
    if (symtable_find(t, string) == NULL)
        printf("OK\n");
    else
        printf("ERROR\n");
}

int main_() {
    printf("START\n");

    struct symtable* t = symtable_init(&free_string);

    char A[10] = "";

    for (int i = 0; i < 30; i++) {
        A[0] = '0' + i / 10;
        A[1] = '0' + i % 10;
        symtable_add(t, A)->value = (void*)make_string(A);
    }

    for (int i = 0; i < 30; i++) {
        A[0] = '0' + i / 10;
        A[1] = '0' + i % 10;
        printf("A %s\n", (char*)symtable_find(t, A)->value);
    }

    //    printf("A %s\n",(char*)symtable_find(t,A)->value);

    symtable_free(t);

    printf("KONEC\n");
    return 0;
}
