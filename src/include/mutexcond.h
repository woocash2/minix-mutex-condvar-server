#ifndef MUTEXCOND_H
#define MUTEXCOND_H

#define CSLOCK_NR 1
#define CSUNLOCK_NR 2
#define CSWAIT_NR 3
#define CSBROADCAST_NR 4

#define LOCK_ACQUIRED 1
#define LOCK_RELEASED 1
#define LOCK_FAIL 2

#define UNLOCK_SUCCESS 1
#define UNLOCK_FAIL 2

#define WAIT_SUCCESS 1
#define WAIT_FAIL 2

#define BROADCAST_SUCCESS 1

#endif