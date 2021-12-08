#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
// 10, 5, 8, 34, 67, 23, 156, 24, 2, 12
    rbtree* t = new_rbtree();

    rbtree_insert(t, 10);
    rbtree_insert(t, 5);
    rbtree_insert(t, 8);
    rbtree_insert(t, 34);

    node_t* find1 = rbtree_max(t);
    printf("find max : %d\n", find1->key);

    node_t* find2 = rbtree_min(t);
    printf("find min : %d\n", find2->key);

    node_t* find3 = rbtree_find(t, 10);
    printf("find 10 : %d\n", find3->key);

    key_t* arr = malloc(5*sizeof(key_t));
    rbtree_to_array(t, arr, 5);
    for (int i=0; i<5; i++){
        printf("%d ", arr[i]);
    }printf("\n");

    rbtree_erase(t, find1);
    printf("erase max\n");
    rbtree_to_array(t, arr, 4);
    for (int i=0; i<4; i++){
        printf("%d ", arr[i]);
    }printf("\n");

    rbtree_erase(t, find2);
    printf("erase min\n");
    rbtree_to_array(t, arr, 3);
    for (int i=0; i<3; i++){
        printf("%d ", arr[i]);
    }printf("\n");


    return 0;
}