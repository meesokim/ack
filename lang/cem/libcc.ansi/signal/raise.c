/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<signal.h>

int kill(int pid, int sig);
int getpid(void);

int
raise(int sig)
{
	if (sig < 0 || sig > _NSIG)
		return -1;
	return kill(getpid(), sig);
}