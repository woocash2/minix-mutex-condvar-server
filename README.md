# Mutex & condition variable server for MINIX

A server which introduces basic process-level synchronization primitives such as **mutexes** and **contition variables** to the MINIX operating system.

## Mutexes

Functions:

* `cs_lock(int mutex_id)` - tries to obtain the mutex of the given ID. If the mutex is not owned, it assigns it to the calling process and returns `0` (success). Otherwise the calling process is suspended until the mutex is eventually assigned to it, and only then the function returns `0` (success).

* `cs_unlock(int mutex_id)` - releases the mutex of the given ID. If the calling process owned the mutex, `0` (success) is returned, and the server assigns the mutex to the next process in the queue. Otherwise `-1` is returned and `errno` is set to `EPERM`.

## Condition variables

Functions:

* `cs_wait(int cond_var_id, int mutex_id)` - if the calling process is the owner of the mutex of the given ID, the mutex is released and the process is suspended. When another process broadcasts an event for the condition variable of the given ID, the process is put in the queue for the mutex, gets unsuspended when it eventually gains back the ownership of the mutex, and the function returns `0` (success). If the calling process is not the owner of the given mutex, the function returns `-1` and sets `errno` to `EINVAL`.

* `cs_broadcast(int cond_var_id)` - causes the server to put all processes waiting for the broadcast event for the given condition variable in the queues for obtaining appropriate mutexes.

## Signals

If a process is suspended waiting for a mutex when receiving a signal, it handles it accordingly and resumes waiting for the mutex afterwards. If a process is waiting for a condition variable broadcast event when receiving a signal, it is put in the appropriate mutex queue and the function returns `0` (success) - spurious wakeup.

Mutexes are immediately released when their owner processes are terminated. 

## Repository structure

The `src` folder corresponds to a fragment of the MINIX 3.2.1 sources, and contains only modified parts. To test, copy the contents of the src directory from the repository to the /usr/src directory in MINIX. As a result, some of the original source files will be overwritten. Then install the includes, recompile and install the standard library and a new server. To enable changes to the basic system servers (PM), the system image must also be recompiled before testing. After rebooting the system, the new functionality should work after starting the newly added server.