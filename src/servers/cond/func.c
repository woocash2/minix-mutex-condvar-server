#include "inc.h"

set mtxset;
set waiting;
list enqueued;

/*===========================================================================*
 *				do_lock				     *
 *===========================================================================*/

int do_lock(message * m) {
    int mutex = m->m1_i1;
    endpoint_t endp = m->m_source;

    list * f = set_find(&mtxset, mutex);
    if (f == NULL) {
        set_add(&mtxset, mutex, endp);
        return 1; // instant success
    }
    else {
        list_add(&enqueued, endp, mutex);
        list_add(f, endp, f->value);
        return 0; // inserted into queue
    }
    // if already has a mutex it will freeze forever
}

/*===========================================================================*
 *				do_unlock				     *
 *===========================================================================*/

int do_unlock(message * m) { // returns pid of the next process to acquire the lock or 0 if there isn't any
    int mutex = m->m1_i1;
    endpoint_t endp = m->m_source;

    list * f = set_find(&mtxset, mutex);
    if (f == NULL || f->endp != endp) {
        return -1;
    }

    // there is such mutex and we are the owner
    m->m_type = 0;

    if (f->front != NULL) {
        endpoint_t next_endp = f->front->endp;
        list_rem_front(f);
        f->endp = next_endp;
        send_response(next_endp, m, LOCK_ACQUIRED);
        list_remove(&enqueued, next_endp);
        return next_endp;
    }

    set_remove(&mtxset, mutex);
    return 0;
}

/*===========================================================================*
 *				do_wait				     *
 *===========================================================================*/

int do_wait(message * m) {
    int mutex = m->m1_i1;
    int cond_var = m->m1_i2;
    endpoint_t endp = m->m_source;

    list * fi = set_find(&mtxset, mutex);
    if (fi == NULL || fi->endp != endp) {
        return -1;
    }

    list * r = set_find(&waiting, cond_var);
    if (r == NULL) {
        r = set_add(&waiting, cond_var, 0);
    }

    list_add(r, endp, mutex);
    do_unlock(m); // release mutex when waiting for broadcast
    return 0;
}

/*===========================================================================*
 *				do_broadcast				     *
 *===========================================================================*/

int do_broadcast(message * m) {
    int cond_var = m->m1_i2;
    endpoint_t endp = m->m_source;

    list * r = set_find(&waiting, cond_var);
    while (r != NULL && r->front != NULL) {
        endpoint_t proc = r->front->endp;
        int mtx = r->front->value;
        list_rem_front(r);

        m->m_source = proc;
        m->m1_i1 = mtx;

        int res = do_lock(m);
        if (res == 1) {
            send_response(proc, m, WAIT_SUCCESS);
        }
    }
    set_remove(&waiting, cond_var); // inefficient
    return 0;
}

/*===========================================================================*
 *				UTILITIES				     *
 *===========================================================================*/

void init_sets() {
    mtxset.front = NULL;
    waiting.front = NULL;
    enqueued.front = NULL;
    enqueued.back = NULL;
}

void send_response(endpoint_t endp, message * m, int type) {
    m->m_type = type;
    int r;
    r = sendnb(endp, m);
}

void put_back_to_mutex_queue(endpoint_t endp) {
    int mutex = list_val_of(&enqueued, endp);
    if (mutex == -1)
        return;
    message m;
    m.m_source = endp;
    m.m1_i1 = mutex;
    do_lock(&m);
}

int remove_from_mutex_queue(endpoint_t endp) {
    if (!list_contains(&enqueued, endp))
        return 0;
    int mutex = list_val_of(&enqueued, endp);
    list * x = set_find(&mtxset, mutex);
    if (x == NULL)
        return 0;
    if (!list_contains(x, endp))
        return 0;
    list_remove(x, endp);
    return 1;
}

void free_locks(endpoint_t endp) {
    list * x = mtxset.front;
    if (x == NULL)
        return;
    list * y = x->next;
    message m;
    while (x != NULL) {
        y = x->next;
        if (x->endp == endp) {  // owner
            m.m_source = endp;
            m.m1_i1 = x->value;
            do_unlock(&m);      // inefficient
        }
        x = y;
    }
}

int wake_up(endpoint_t endp) {
    list * x = waiting.front;
    while (x != NULL && !list_contains(x, endp))
        x = x->next;
    if (x == NULL)
        return 0;
    int mutex = list_remove(x, endp);
    message m;
    m.m_source = endp;
    m.m1_i1 = mutex;
    int res = do_lock(&m);
    if (res == 1) { // instantly locked
        send_response(endp, &m, LOCK_ACQUIRED); // same as WAIT_SUCCESS
    }
    return 1;
}