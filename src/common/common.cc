
#include "common.h"


#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <signal.h>
#include <stdio.h>


int GetCmdline(char *out, int n)
{
	int fd;
	if ((fd = open("/proc/self/cmdline", O_RDONLY)) > 0)
	{
		if ((n = read(fd, out, n)) > 0)
		{
			for (int i = 0; i < n; ++i)
			{
				if (out[i] == 0)
				{
					out[i] = '_';
				}
			}
			out[n] = 0;
			return n;
		}
	}
	return 0;
}

int GetCurrentTimeFmtStr(char *out, int n, const char *fmt)
{
	time_t time_v;
	time(&time_v);
	struct tm * tm = localtime(&time_v);
	return strftime(out, n, fmt, tm);
}


bool IsSameDay(time_t t1, time_t t2)
{
	struct tm tm1, tm2;
	localtime_r(&t1, &tm1);
	localtime_r(&t2, &tm2);

	if (tm1.tm_year == tm2.tm_year && tm1.tm_yday == tm2.tm_yday)
		return true;

	return false;
}
const char* GetBasename(const char *file)
{
	const char *base = strrchr(file, '/');
	base = base ? base + 1 : file;
	return base;
}


void daemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	/*
	 *          * Clear file creation mask.
	 *                   */
	umask(0);

	/*
	 *          * Get maximum number of file descriptors.
	 *                   */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		fprintf(stderr, "%s: can't get file limit", cmd);

	/*
	 *          * Become a session leader to lose controlling TTY.
	 *                   */
	if ((pid = fork()) < 0)
		fprintf(stderr, "%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);
	setsid();

	/*
	 *          * Ensure future opens won't allocate controlling TTYs.
	 *                   */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		fprintf(stderr, "%s: can't ignore SIGHUP", cmd);
	if ((pid = fork()) < 0)
		fprintf(stderr, "%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);

	/*
	 *          * Change the current working directory to the root so
	 *                   * we won't prevent file systems from being unmounted.
	 *                            */
	//if (chdir("/") < 0)
	//	fprintf(stderr, "%s: can't change directory to /", cmd);

	/*
	 *          * Close all open file descriptors.
	 *                   */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 1; i < rl.rlim_max; i++)
		close(i);

	/*
	 *          * Attach file descriptors 0, 1, and 2 to /dev/null.
	 *                   */
	//fd0 = open("/dev/null", O_RDWR);
	//fd1 = dup(0);
	//fd2 = dup(0);

	/*
	 *          * Initialize the log file.
	 *                   */
	//openlog(cmd, LOG_CONS, LOG_DAEMON);
	//if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
	//	syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
	//			fd0, fd1, fd2);
	//	exit(1);
	//}
}
