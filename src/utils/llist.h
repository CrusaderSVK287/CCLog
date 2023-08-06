#ifndef __CCLOG_LLIST_H__
#define __CCLOG_LLIST_H__

/**
 * Node of linked list
 */
typedef struct cclog_llist_node {
    void *data;
    struct cclog_llist_node *next;
} cclog_llist_node_t;

/**
 * linked list base structure
 */
typedef struct cclog_llist {
    cclog_llist_node_t *first;
    int num_of_entries;
} cclog_llist_t;

/**
 * Returns an linked list with one node containing data
 */
cclog_llist_t *llist_init(void *data);

/**
 * appends new allocated node to the list and fills it with data
 */
int llist_add(cclog_llist_t *list, void *data);

/**
 * Returns data on specified index, this data must be casted
 */
void *llist_get_index(cclog_llist_t *list, int index);

/**
 * Frees every node AND ITS DATA, frees the list itself, does NOT set list to NULL
 */
void llist_clean(cclog_llist_t *list);

#endif /* __CCLOG_LLIST_H__ */

