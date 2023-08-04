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
 * Returns an empty linked list
 */
cclog_llist_t *llist_init(void *data);

/**
 * adds data to list
 */
int llist_add(cclog_llist_t *list, void *data);

/**
 * Returns data on specified index, this data must be casted
 */
void *llist_get_index(cclog_llist_t *list, int index);

/**
 * Clears the entire list from heap
 */
void llist_clean(cclog_llist_t *list);

#endif /* __CCLOG_LLIST_H__ */

