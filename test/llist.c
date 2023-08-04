#include "tests.h"
#include "../src/utils/llist.h"
#include <stdio.h>

void llist_test()
{
        int a = 15;
        int b = 28;
        int c = 13;
        int d = 51;

        int *res = NULL;

        cclog_llist_t *llist = llist_init(&a);

        llist_add(llist, &b);
        llist_add(llist, &c);
        llist_add(llist, &d);

        for(int i = 0; i < 4; i++) {
                res = (int *)llist_get_index(llist, i);
                printf("llist test #%d: Result: %d\n", i+1, *res);
        }

        res = (int *)llist_get_index(llist,2);
        printf("llist test #5: Expected 13, Result: %d\n", *res);

        res = (int *)llist_get_index(llist, 36);
        printf("llist test #6: Expected 51, Result: %d\n", *res);

        llist_clean(llist);
}

