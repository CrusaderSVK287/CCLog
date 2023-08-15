#include "llist.h"
#include <stdlib.h>

/* Function returns the node at specified index or NULL if index is invalid */
static cclog_llist_node_t *get_node_index(cclog_llist_node_t *first, int target)
{
        if (!first || target < 0)
                return NULL;

        int i = 0;
        while (i != target) {
                first = first->next;
                i++;
                
                if (!first)
                        return NULL;
        }

        return first;
}

cclog_llist_t *llist_init(void *data)
{
        if (!data)
                return NULL;

        /* allocate the space on heap for list and first node, then fill the node */
        cclog_llist_t *llist = calloc(sizeof(cclog_llist_t), 1);

        llist->first = calloc(sizeof(cclog_llist_node_t), 1);
        llist->first->next = NULL;
        llist->first->data = data;
        llist->num_of_entries = 1;

        return llist;
}

int llist_add(cclog_llist_t *list, void *data)
{
        if (!list || !data)
                return -1;

        /* get the last node (index is from 0, num_of_entries is from 1)*/
        cclog_llist_node_t *node = get_node_index(list->first, list->num_of_entries -1);
        
        if (!node)
                return -1;

        /* allocate new node and fill with data */
        node->next = calloc(sizeof(cclog_llist_node_t), 1);
        if (!node->next)
                return -1;

        node->next->data = data;
        node->next->next = NULL;

        /* increment num_of_entries */
        list->num_of_entries++;

        return 0;
}

void *llist_get_index(cclog_llist_t *list, int index)
{
        /* get node on index and return its data */
        cclog_llist_node_t *node = get_node_index(list->first, index);
        return (node) ? node->data : NULL;
}

void llist_clean(cclog_llist_t *list)
{
        cclog_llist_node_t *this = list->first;
        cclog_llist_node_t *tmp;

        /* free every node AND ITS DATA then free the list */
        while(this) {
                tmp = this->next;
                if (this->data)
                        free(this->data);
                free(this);
                this = tmp;
        }

        free(list);
}

void llist_foreach(cclog_llist_t *llist, llist_foreach_func func, void *priv)
{
        if (!llist || !func)
                return;

        for (int i = 0; i < llist->num_of_entries; i++) {
                func(llist_get_index(llist, i), priv);
        }
}

