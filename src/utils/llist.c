#include "llist.h"
#include <stdlib.h>

static cclog_llist_node_t *get_node_index(cclog_llist_node_t *first, int target)
{
        if (!first)
                return NULL;

        int i = 0;
        while (i != target && first->next != NULL) {
                first = first->next;
                i++;
        }

        return first;
}

cclog_llist_t *llist_init(void *data)
{
        if (!data)
                return NULL;

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

        cclog_llist_node_t *node = get_node_index(list->first, list->num_of_entries -1);
        
        if (!node)
                return -1;

        node->next = calloc(sizeof(cclog_llist_node_t), 1);
        if (!node->next)
                return -1;

        node->next->data = data;
        node->next->next = NULL;

        list->num_of_entries++;

        return 0;
}

void *llist_get_index(cclog_llist_t *list, int index)
{
        return get_node_index(list->first, index)->data;
}

void llist_clean(cclog_llist_t *list)
{
        cclog_llist_node_t *this = list->first;
        cclog_llist_node_t *tmp;

        while(this) {
                tmp = this->next;
                free(this);
                this = tmp;
        }

        free(list);
}

