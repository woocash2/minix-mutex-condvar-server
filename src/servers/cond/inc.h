#ifndef INC_H
#define INC_H

#define _POSIX_SOURCE      1	/* tell headers to include POSIX stuff */
#define _MINIX             1	/* tell headers to include MINIX stuff */
#define _SYSTEM            1    /* get OK and negative error codes */

#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <machine/vm.h>
#include <machine/vmparam.h>
#include <sys/vm.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include "mutexcond.h"

typedef struct node {
    endpoint_t endp;    // endpoint
    int value;          // optional value to store
    struct node * next; // pointer to next
} node;

typedef struct list {
    endpoint_t endp;    // endpoint
    int value;          // optional main value to store
    node * front;       // pointer to front node
    node * back;        // pointer to back node
    struct list * next; // pointer to next list
} list;

typedef struct set { // wrapper for list of lists.
    list * front;
} set;

EXTERN int identifier;
EXTERN endpoint_t who_e;
EXTERN int call_type;
EXTERN endpoint_t SELF_E;

node * list_add(list * fi, endpoint_t endp, int val);
void list_rem_front(list * fi);
int list_remove(list * fi, endpoint_t endp);
void list_free(list * fi);
int list_val_of(list * fi, endpoint_t endp);
int list_contains(list * fi, endpoint_t endp);

list * set_find(set * set, int val);
list * set_add(set * set, int val, endpoint_t endp);
void set_remove(set * set, int val);

int do_lock(message * m);
int do_unlock(message * m);
int do_wait(message * m);
int do_broadcast(message * m);

void init_sets();
void send_response(endpoint_t endp, message * m, int type);
int remove_from_mutex_queue(endpoint_t endp);
void free_locks(endpoint_t endp);
int wake_up(endpoint_t endp);

#endif