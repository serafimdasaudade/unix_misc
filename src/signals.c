#include "log.h"
#include "signals.h"

int block_all_signals(FILE *fplog)
{
	int rc;
	sigset_t new_mask;
        sigset_t old_mask;

        sigfillset(&new_mask);
        sigdelset (&new_mask, SIGFPE);
        sigdelset (&new_mask, SIGILL);
        sigdelset (&new_mask, SIGSEGV);
        sigdelset (&new_mask, SIGBUS);
        rc=pthread_sigmask (SIG_BLOCK, &new_mask, &old_mask);
        if (rc)
        {
                log_printf(fplog,LOG_ERROR,"block_all_signals: %s (%d)",strerror(errno),errno);
                return(-1);
        }
	return(0);
}

int block_signal(FILE *fplog,int sig)
{
	int rc;
        sigset_t sig_mask;
        sigemptyset(&sig_mask);
        sigaddset(&sig_mask, sig);
        rc=pthread_sigmask(SIG_BLOCK, &sig_mask, NULL);
        if (rc)
        {
                log_printf(fplog,LOG_ERROR,"block_signal: %s (%d)",strerror(errno),errno);
                return(-1);
        }
	return(0);
}

int unblock_signal(FILE *fplog,int sig)
{
	int rc;
        sigset_t sig_mask;
        sigemptyset(&sig_mask);
        sigaddset(&sig_mask, sig);
        rc=pthread_sigmask(SIG_UNBLOCK, &sig_mask, NULL);
        if (rc)
        {
                log_printf(fplog,LOG_ERROR,"unblock_signal: %s (%d)",strerror(errno),errno);
                return(-1);
        }
	return(0);
}

int set_signal(FILE *fplog,int sig,void (*func)(int))
{
	int rc;
        struct sigaction act;
        act.sa_handler = func;
        rc=sigaction(sig, &act, NULL);
        if (rc)
        {
                log_printf(fplog,LOG_ERROR,"signal: %s (%d)",strerror(errno),errno);
                return(-1);
        }
	return(0);
}

