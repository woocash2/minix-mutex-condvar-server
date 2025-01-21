#include "inc.h"

node * list_add(list * fi, endpoint_t endp, int val) { // adding to back

    // create new node struct
    node * y = (node *) malloc(sizeof(node));
    y->value = val;
    y->endp = endp;
    y->next = NULL;

    // if list is empty
    if (fi->back == NULL) {
        fi->front = fi->back = y;
    }
    else { // non-empty
        fi->back->next = y;
        fi->back = y;
    }

    return y;
}

int list_remove(list * fi, endpoint_t endp) {
    if (fi->front == NULL)
        return -1;

    if (fi->front->endp == endp) {
        int val = fi->front->value;
        node * x = fi->front->next;
        free(fi->front);
        fi->front = x;
        if (x == NULL)
            fi->back = NULL;
        return val;
    }

    node * y = fi->front;
    while (y->next != NULL && y->next->endp != endp)
        y = y->next;

    if (y->next == NULL)
        return -1;

    node * x = y->next;
    y->next = x->next;
    int val = x->value;
    if (x->next == NULL)
        fi->back = y;

    free(x);
    return val;
}

void list_rem_front(list * fi) { // removes the first element of the list
    node * nd = fi->front->next;
    free(fi->front);
    fi->front = nd;
    if (nd == NULL)
        fi->back = NULL;
}

void list_free(list * fi) {
    while (fi->front != NULL)
        list_rem_front(fi);
    free(fi);
}

int list_contains(list * fi, endpoint_t endp) {
    if (fi->front == NULL)
        return 0;
    node * x = fi->front;
    while (x != NULL && x->endp != endp)
        x = x->next;
    if (x == NULL)
        return 0;
    return 1;
}

int list_val_of(list * fi, endpoint_t endp) {
    if (fi->front == NULL)
        return -1;
    node * x = fi->front;
    while (x != NULL && x->endp != endp)
        x = x->next;
    if (x == NULL)
        return -1;
    return x->value;
}

list * set_find(set * set, int val) {
    list * f = set->front;
    while (f != NULL) {
        if (f->value == val) {
            return f;
        }
        f = f->next;
    }
    return NULL;
}

list * set_add(set * set, int val, endpoint_t endp) { // adding to front
    list * f = (list *) malloc(sizeof(list));
    f->front = f->back = NULL;
    f->next = NULL;
    f->value = val;
    f->endp = endp;

    if (set->front == NULL) {
        set->front = f;
    }
    else {
        f->next = set->front;
        set->front = f;
    }
    return f;
}

void set_remove(set * set, int val) {
    if (set->front == NULL)
        return;
    list * f = set->front;
    if (f->value == val) {
        list * g = f->next;
        set->front = g;
        list_free(f);
    }
    else {
        while (f->next != NULL && f->next->value != val) {
            f = f->next;
        }
        if (f->next == NULL)
            return;
        list * g = f->next->next;
        list_free(f->next);
        f->next = g;
    }
}

