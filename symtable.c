/**
 * @project Compiler IFJ22
 * @file    symtable.c
 * @authors  Štěpán Nekula <xnekul04>, Matej Vadovič <xvadov01>
 */

#include "symtable.h"

symtable_t *symtable_init(void (*func)(void *)) {
    symtable_t *ptr = malloc(sizeof(symtable_t));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->count = 0;
    ptr->head = NULL;
    ptr->free_func = func;
    return ptr;
}

void symtable_free(symtable_t *t) {
    symtable_clear(t);
    free(t);
}

void symtable_clear(symtable_t *t) {
    if (t == NULL) return;
    while (t->head != NULL) {
        symtable_remove_frame(t);
    }
}

int symtable_add_frame(symtable_t *t) {
    if (t == NULL) return 1;
    t->count++;
    symtable_item_t *tmp = t->head;
    t->head = malloc(sizeof(symtable_item_t));
    if (t->head == NULL) {
        t->head = tmp;
        return 1;
    }
    t->head->next = tmp;

    t->head->hash_table = htab_init(HTAB_INIT_VALUE);
    return 0;
}

void symtable_remove_frame(symtable_t *t) {
    if (t == NULL) return;
    if (t->head == NULL) return;
    t->count--;
    symtable_item_t *tmp = t->head;
    t->head = tmp->next;

    htab_free(tmp->hash_table, t->free_func);
    free(tmp);
}

htab_pair_t *symtable_find(symtable_t *t, htab_key_t key) {
    if (t == NULL) return NULL;
    htab_pair_t *found_value = NULL;
    symtable_item_t *node = t->head;
    if (node == NULL) {
        return NULL;
    }
    found_value = htab_find(node->hash_table, key);
    if (found_value != NULL) return found_value;

    return NULL;
}

htab_pair_t *symtable_add(symtable_t *t, htab_key_t key, value_t *(*value_create)()) {
    if (t == NULL) return NULL;
    if (t->head == NULL) {
        if (symtable_add_frame(t)) {
            return NULL;
        }
    }

    return htab_lookup_add(t->head->hash_table, key, value_create);
}

size_t htab_hash_function(const char *str) {
    if (str == NULL) return 0;

    uint32_t h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 65599 * h + *p;
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

void htab_free(htab_t *t, void (*f)(void *)) {
    htab_clear(t, f);
    free(t->arr_ptr);
    free(t);
}

void htab_clear(htab_t *t, void (*f)(void *)) {
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

size_t htab_size(const htab_t *t) {
    if (t == NULL) return 0;
    return t->size;
}

size_t htab_bucket_count(const htab_t *t) {
    if (t == NULL) return 0;
    return t->arr_size;
}

void htab_resize(htab_t *t, size_t newn) {
    if (t == NULL) return;
    if (newn < MIN_TABLE_SIZE) newn = MIN_TABLE_SIZE;

    htab_t *newt = htab_init(newn);
    if (newt == NULL) return;

    htab_item_t *start = NULL;
    size_t position = 0;
    (void)position;
    htab_pair_t *change = NULL;

    for (size_t i = 0; i < t->arr_size; i++) {
        start = t->arr_ptr[i];

        for (htab_item_t *j = start; j != NULL; j = j->next) {
            change = htab_lookup_add(newt, j->data.key, &value_create_null);
            change->value = j->data.value;
        }
    }

    void (*f)() = &nope;
    htab_clear(t, f);
    free(t->arr_ptr);
    t->arr_ptr = newt->arr_ptr;
    free(newt);
    t->arr_size = newn;
    return;
}

htab_pair_t *htab_find(htab_t *t, htab_key_t key) {
    if (t == NULL || key == NULL) return NULL;
    size_t position = (htab_hash_function(key) % t->arr_size);
    for (htab_item_t *i = t->arr_ptr[position]; i != NULL; i = i->next) {
        if (!strcmp(key, i->data.key)) return &(i->data);
    }
    return NULL;
}

htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key, value_t *(*value_create)()) {
    if (t == NULL || key == NULL) return NULL;
    size_t position = (htab_hash_function(key) % t->arr_size);
    htab_item_t *last = NULL;

    for (htab_item_t *i = t->arr_ptr[position]; i != NULL; i = i->next) {
        if (!strcmp(key, i->data.key)) {
            return &(i->data);
        }
        last = i;
    }
    htab_item_t *newitem = malloc(sizeof(htab_item_t));
    if (newitem == NULL) return NULL;
    newitem->data.key = malloc(strlen(key) + 1);
    if (newitem->data.key == NULL) return NULL;
    strcpy((char *)newitem->data.key, (char *)key);
    newitem->next = NULL;
    newitem->data.value = (*value_create)();

    if (t->arr_ptr[position] == NULL) {
        t->arr_ptr[position] = newitem;
    } else {
        last->next = newitem;
    }
    t->size++;

    if (t->size / t->arr_size > AVG_LEN_MAX) {
        htab_resize(t, 2 * t->arr_size);
        return htab_find(t, key);
    }

    return &(newitem->data);
}

void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data)) {
    htab_item_t *start;
    for (size_t i = 0; i < t->arr_size; i++) {
        start = t->arr_ptr[i];
        for (htab_item_t *j = start; j != NULL; j = j->next) {
            f(&(j->data));
        }
    }
}

void nope(void) {}

value_t *value_create_fnc() {
    value_t *ptr = (value_t *)malloc(sizeof(value_t));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->type = value_fnc;
    value_fnc_t *fn = &ptr->val.fnc;
    fn->parameters = NULL;
    fn->number_of_parameters = 0;
    fn->return_type = "void";
    return ptr;
}

value_t *value_create_var() {
    value_t *ptr = (value_t *)malloc(sizeof(value_t));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->type = value_var;
    ptr->val.var.var_type = T_NO_TYPE;
    return ptr;
}

value_t *value_create_null() {
    return NULL;
}
