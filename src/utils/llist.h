#ifndef __CCLOG_LLIST_H__
#define __CCLOG_LLIST_H__

typedef int (*llist_foreach_func)(void *data, void *priv);

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

/**
 * Function loops through all nodes in a list and calls func with the node 
 * and priv data as parameters
 */
void llist_foreach(cclog_llist_t *llist, llist_foreach_func func, void *priv);

#endif /* __CCLOG_LLIST_H__ */

