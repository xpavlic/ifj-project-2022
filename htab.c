
#include "htab.h"

#include "stdint.h"

htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key) {
    if (t == NULL || key == NULL) return NULL;
    size_t position = (htab_hash_function(key) % t->arr_size);
    htab_item_t *last = NULL;
    // check if already existts
    for (htab_item_t *i = t->arr_ptr[position]; i != NULL; i = i->next) {
        if (!strcmp(key, i->data.key)) {
            return &(i->data);
        }
        last = i;
    }
    // if doesnt exists
    // create new item
    htab_item_t *newitem = malloc(sizeof(htab_item_t));
    if (newitem == NULL) return NULL;
    newitem->data.key = malloc(strlen(key) + 1);
    if (newitem->data.key == NULL) return NULL;
    // setup new item
    strcpy((char *)newitem->data.key, (char *)key);
    newitem->next = NULL;
    newitem->data.value = value_init();

    // place new item do seznamu
    if (t->arr_ptr[position] == NULL) {
        t->arr_ptr[position] = newitem;
    } else {
        last->next = newitem;
    }
    t->size++;
    // resize
    if (t->size / t->arr_size > AVG_LEN_MAX) {
        htab_resize(t, 2 * t->arr_size);
        return htab_find(t, key);
    }

    return &(newitem->data);
}

// File::Bucket
size_t htab_bucket_count(const htab_t *t)  // velikost pole
{
    if (t == NULL) return 0;
    return t->arr_size;
}

// File::Clear

void htab_clear(htab_t *t, void (*f)(void *))  // ruší všechny záznamy
{
    htab_item_t *start = NULL;
    htab_item_t *tmpnext = NULL;
    (void)tmpnext;
    for (size_t i = 0; i < t->arr_size; i++) {
        start = t->arr_ptr[i];
        if (start == NULL) continue;
        for (htab_item_t *j = start; j != NULL;) {
            htab_item_t *tmpnext = j->next;
            free((char *)j->data.key);
            f(j->data.value);
            free(j);
            j = tmpnext;
        }
        t->arr_ptr[i] = NULL;
    }
}

// File::find
htab_pair_t *htab_find(htab_t *t, htab_key_t key)  // hledání
{
    if (t == NULL || key == NULL) return NULL;
    size_t position = (htab_hash_function(key) % t->arr_size);
    for (htab_item_t *i = t->arr_ptr[position]; i != NULL; i = i->next) {
        if (!strcmp(key, i->data.key)) return &(i->data);
    }
    return NULL;
}

// File::foreach
void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data)) {
    htab_item_t *start;
    for (size_t i = 0; i < t->arr_size; i++) {
        start = t->arr_ptr[i];
        for (htab_item_t *j = start; j != NULL; j = j->next) {
            f(&(j->data));
        }
    }
}

void htab_free(htab_t *t, void (*f)(void *))  // destruktor tabulky
{
    htab_clear(t, f);
    free(t->arr_ptr);
    free(t);
}

size_t htab_hash_function(const char *str) {
    if (str == NULL) return 0;

    uint32_t h = 0;  // musí mít 32 bitů
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++) h = 65599 * h + *p;
    return h;
}

htab_t *htab_init(size_t n) {
    if (n < MIN_TABLE_SIZE) n = MIN_TABLE_SIZE;

    htab_t *mytab = malloc(sizeof(htab_t));
    if (mytab == NULL) return NULL;
    mytab->size = 0;
    mytab->arr_size = n;
    mytab->arr_ptr = NULL;

    mytab->arr_ptr = calloc(n, sizeof(htab_item_t *));
    if (mytab->arr_ptr == NULL) {
        free(mytab);
        return NULL;
    }
    return mytab;
}

void htab_resize(htab_t *t, size_t newn)  // změna velikosti pole
{
    // arg check                                     // (umožňuje rezervaci
    // místa)
    if (t == NULL) return;
    if (newn < MIN_TABLE_SIZE) newn = MIN_TABLE_SIZE;

    // mallock
    htab_t *newt = htab_init(newn);
    if (newt == NULL) return;

    htab_item_t *start = NULL;
    size_t position = 0;
    (void)position;
    htab_pair_t *change = NULL;

    // through ptr array
    for (size_t i = 0; i < t->arr_size; i++) {
        start = t->arr_ptr[i];
        // through seznam
        for (htab_item_t *j = start; j != NULL; j = j->next) {
            change = htab_lookup_add(newt, j->data.key);
            change->value = j->data.value;
        }
    }
    // remove old tab
    void (*f)() = &nope;
    htab_clear(t, f);
    free(t->arr_ptr);
    t->arr_ptr = newt->arr_ptr;
    free(newt);
    t->arr_size = newn;
    return;
}

size_t htab_size(const htab_t *t)  // počet záznamů v tabulce
{
    if (t == NULL) return 0;
    return t->size;
}

void nope(void) {}  // does nothing

value_t *value_init() {
    value_t *ptr = (value_t *)malloc(sizeof(value_t));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->array = (enum tree_node_type *)malloc(sizeof(int));
    if (ptr->array == NULL) {
        return NULL;
    }
    ptr->number_of_parameters = 0;
    return ptr;
}
