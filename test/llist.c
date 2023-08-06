#include "tests.h"
#include "../src/utils/llist.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct s {
        int a;
        int b;
} s_t;

void add_to_list(cclog_llist_t* llist)
{
        s_t *test_struct = malloc(sizeof(s_t));
        test_struct->a = 5;
        test_struct->b = 8;

        llist_add(llist, test_struct);
}

void llist_test()
{
        printf("\n\n starting linked list tests\n\n");
        int *a = malloc(sizeof(int));
        int *b = malloc(sizeof(int));
        int *c = malloc(sizeof(int));
        int *d = malloc(sizeof(int));

        *a = 15;
        *b = 27;
        *c = 13;
        *d = 52;

        int *res = NULL;

        cclog_llist_t *llist = llist_init(a);

        llist_add(llist, b);
        llist_add(llist, c);
        llist_add(llist, d);
        add_to_list(llist);

        for(int i = 0; i < 4; i++) {
                res = (int *)llist_get_index(llist, i);
                printf("llist test #%d: Result: %d\n", i+1, *res);
        }

        res = (int *)llist_get_index(llist,2);
        printf("llist test #5: Expected 13, Result: %d\n", *res);


        s_t *test_struct = (s_t *)llist_get_index(llist, 5);
        printf("llist test #6: Expected NULL, Result: %s\n", test_struct? "Not null" : "NULL");

        llist_clean(llist);
}

