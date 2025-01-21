#include "inc.h"

int identifier;
endpoint_t who_e;
int call_type;
endpoint_t SELF_E;


/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);
static void sef_cb_signal_handler(int signo);


int main(int argc, char *argv[])
{
	message m;

	/* SEF local startup. */
	env_setargs(argc, argv);
	sef_local_startup();
	init_sets();

	while (TRUE) {
		int r;

		if ((r = sef_receive(ANY, &m)) != OK)
			printf("sef_receive failed %d.\n", r);

		who_e = m.m_source;
		call_type = m.m_type;

		if (call_type == PM_DUMPCORE || call_type == PM_EXIT) {
            endpoint_t endp = m.PM_PROC;
            wake_up(endp);
            remove_from_mutex_queue(endp);
            free_locks(endp);
            continue;
		}
		if (m.m_type == PM_UNPAUSE) {
            endpoint_t endp = m.PM_PROC;
            int res = remove_from_mutex_queue(endp);
            if (res == 1)
                send_response(endp, &m, LOCK_FAIL);
            wake_up(endp);
            continue;
		}

		int result;

        if (call_type == CSLOCK_NR) {
            result = do_lock(&m);
		    if (result == 1) // instantly locked
		        send_response(who_e, &m, LOCK_ACQUIRED);
		}
		else if (call_type == CSUNLOCK_NR) {
            result = do_unlock(&m);

            if (result == -1)
                send_response(who_e, &m, UNLOCK_FAIL);
            else
                send_response(who_e, &m, UNLOCK_SUCCESS);
		}
		else if (call_type == CSWAIT_NR) {
		    result = do_wait(&m);
		    if (result < 0)
		        send_response(who_e, &m, WAIT_FAIL);
		}
		else if (call_type == CSBROADCAST_NR) {
            result = do_broadcast(&m);
            send_response(who_e, &m, BROADCAST_SUCCESS);
        }
		else {
            fprintf(stderr, "Bad message type");
            continue;
        }
	}

	return -1;
}

/*===========================================================================*
 *			       sef_local_startup			     *
 *===========================================================================*/
static void sef_local_startup() {
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_restart(sef_cb_init_fresh);

  /* No live update support for now. */

  /* Register signal callbacks. */
  sef_setcb_signal_handler(sef_cb_signal_handler);

  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
static int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *UNUSED(info))
{
/* Initialize the ipc server. */

  SELF_E = getprocnr();
  return(OK);
}

/*===========================================================================*
 *		            sef_cb_signal_handler                            *
 *===========================================================================*/
static void sef_cb_signal_handler(int signo)
{

}
