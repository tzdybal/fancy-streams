#include <unistd.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int out_fds[2];
	int err_fds[2];
	int status;
	pid_t pid;

	status = pipe(out_fds);
	if (status == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	status = pipe(err_fds);
	if (status == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	if (pid == 0) {
		/* child process */
		close(out_fds[0]);
		close(err_fds[0]);

		dup2(out_fds[1], STDOUT_FILENO);
		dup2(err_fds[1], STDERR_FILENO);

		execlp("find", "find", "/root", NULL);
	} else {
		/* parent process */
		struct epoll_event ev, events[2];
		int epollfd;
		int eofs = 0;

		close(out_fds[1]);
		close(err_fds[1]);

		epollfd = epoll_create1(0);

		ev.events = EPOLLIN;

		ev.data.fd = out_fds[0];
		epoll_ctl(epollfd, EPOLL_CTL_ADD, out_fds[0], &ev);
		
		ev.data.fd = err_fds[0];
		epoll_ctl(epollfd, EPOLL_CTL_ADD, err_fds[0], &ev);

		while (eofs < 2) {
			int n;
			int nfds = epoll_wait(epollfd, events, 2, -1);

			for (n = 0; n < nfds; ++n) {
				int color = 32;
				char buf[80];
				ssize_t s = read(events[n].data.fd, &buf, sizeof(buf)-1);
				if (s > 0) {
					buf[s] = '\0';

					if (events[n].data.fd == err_fds[0])
						color = 31;
					printf("\033[0;%dm%s\033[0m", color, buf);
				} else {
					++eofs;
				}
			}
		}

		wait(&status);
		return status;
	}

	return EXIT_SUCCESS;
}
