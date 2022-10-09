#pragma once

#include "hashtable.h"
#include "hashtable_private.h" /* needed to enable inlining */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* This struct is only concrete here to allow the inlining of two of the
 * accessor functions. */
struct hashtable_itr
{
    struct hashtable *h;
    struct entry *e;
    struct entry *parent;
    unsigned int index;
};


/*****************************************************************************/
/* hashtable_iterator
 */

struct hashtable_itr *
hashtable_iterator(struct hashtable *h);

/*****************************************************************************/
/* hashtable_iterator_key
 * - return the value of the (key,value) pair at the current position */

static inline void *
hashtable_iterator_key(struct hashtable_itr *i)
{
    return i->e->k;
}

/*****************************************************************************/
/* value - return the value of the (key,value) pair at the current position */

static inline void *
hashtable_iterator_value(struct hashtable_itr *i)
{
    return i->e->v;
}

/*****************************************************************************/
/* advance - advance the iterator to the next element
 *           returns zero if advanced to end of table */

int
hashtable_iterator_advance(struct hashtable_itr *itr);

/*****************************************************************************/
/* remove - remove current element and advance the iterator to the next element
 *          NB: if you need the value to free it, read it before
 *          removing. ie: beware memory leaks!
 *          returns zero if advanced to end of table */

int
hashtable_iterator_remove(struct hashtable_itr *itr);

/*****************************************************************************/
/* search - overwrite the supplied iterator, to point to the entry
 *          matching the supplied key.
            h points to the hashtable to be searched.
 *          returns zero if not found. */
int
hashtable_iterator_search(struct hashtable_itr *itr,
                          struct hashtable *h, void *k);

#define DEFINE_HASHTABLE_ITERATOR_SEARCH(fnname, keytype) \
int fnname (struct hashtable_itr *i, struct hashtable *h, keytype *k) \
{ \
    return (hashtable_iterator_search(i,h,k)); \
}


#ifdef __cplusplus
}
#endif
