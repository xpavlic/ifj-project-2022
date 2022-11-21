
// htab.h -- rozhraní knihovny htab (řešení IJC-DU2)
// Licence: žádná (Public domain)
// Autor: Štěpán Nekula


#ifndef __HTAB_H__
#define __HTAB_H__

#include <string.h>     // size_t
#include <stdbool.h>    // bool
//#include <stdint-gcc.h>
#include "structs.c"

// Table:
struct htab;
typedef struct htab htab_t;

/**
 * @brief hash function for htab
 * @param htab_key_t str
 * @return size_t
*/
size_t htab_hash_function(htab_key_t str);

// Functions for manimulating table:

/**
 * @brief htable constructor
 * @param size_t n
 * @return htab_t*
*/
htab_t *htab_init(size_t n);                    // konstruktor tabulky

/**
 * @brief returns number of records in table
 * @param const htab_t * t
 * @return size_t
*/
size_t htab_size(const htab_t * t);             // počet záznamů v tabulce

/**
 * @brief returns size of array
 * @param const htab_t * t
 * @return size_t
*/
size_t htab_bucket_count(const htab_t * t);     // velikost pole

/**
 * @brief changes size of table
 * @param htab_t *t
 * @param size_t newn
*/
void htab_resize(htab_t *t, size_t newn);       // změna velikosti pole
                                                // (umožňuje rezervaci místa)
/**
 * @brief finds pair in table
 * @param const htab_t * t
 * @param htab_key_t key
 * @return htab_pair_t *
*/
htab_pair_t * htab_find(htab_t * t, htab_key_t key);  // hledání

/**
 * @brief adds pair into table
 * @param const htab_t * t
 * @param htab_key_t key
 * @return htab_pair_t *
*/
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);

// for_each: projde všechny záznamy a zavolá na ně funkci f
// Pozor: f nesmí měnit klíč .key ani přidávat/rušit položky
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));

/**
 * @brief removes all records in table
 * @param htab_t * t
 * @param void (*f)(void*)
*/
void htab_clear(htab_t * t,void (*f)(void*));    // ruší všechny záznamy

/**
 * @brief table destructor
 * @param htab_t * t
 * @param void (*f)(void*)
*/
void htab_free(htab_t * t,void (*f)(void*));     // destruktor tabulky

/**
 * @brief does nothing
*/
void nope(void); //does nothing


#endif // __HTAB_H__
