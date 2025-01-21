#include "namespace.h"
#include <sys/cdefs.h>
#include <lib.h>
#include <unistd.h>
#include "stdio.h"
#include "errno.h"
#include "mutexcond.h"
#include "cond.h"

int cs_lock(int mutex) {
    message m;
    m.m1_i1 = mutex;

    endpoint_t endpoint;

    if (minix_rs_lookup("cond", &endpoint) != 0) { // change 0 to OK
        fprintf(stderr, "Can't find COND");
        return -1;
    }

    int done = 0;
    while (!done) {
        int retval = _syscall(endpoint, CSLOCK_NR, &m);
        if (m.m_type == LOCK_ACQUIRED) {
            done = 1;
        }
    }

    return 0;
}

int cs_unlock(int mutex) {
    message m;
    m.m1_i1 = mutex;

    endpoint_t endpoint;

    if (minix_rs_lookup("cond", &endpoint) != 0) { // change 0 to OK
        return -1;
    }

    int retval = _syscall(endpoint, CSUNLOCK_NR, &m);
    if (m.m_type == UNLOCK_FAIL) {
        errno = EPERM;
        return -1;
    }
    // else UNLOCK_SUCCESS
    return 0;
}

int cs_wait(int cond_var, int mutex) {
    message m;
    m.m1_i1 = mutex;
    m.m1_i2 = cond_var;

    endpoint_t endpoint;
    if (minix_rs_lookup("cond", &endpoint) != 0) {
        fprintf(stderr, "Can't find COND");
        return -1;
    }

    int retval = _syscall(endpoint, CSWAIT_NR, &m);

    if (m.m_type == WAIT_FAIL) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

int cs_broadcast(int cond_var) {
    message m;
    m.m1_i2 = cond_var;

    endpoint_t endpoint;
    if (minix_rs_lookup("cond", &endpoint) != 0) {
        fprintf(stderr, "Can't find COND");
        return -1;
    }

    int retval = _syscall(endpoint, CSBROADCAST_NR, &m);
    return 0;
}