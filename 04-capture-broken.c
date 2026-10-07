/* Part 3: capture a child's output through a pipe, the WRONG way round.
 * The parent calls wait() first and reads afterwards. Once the child has
 * filled the pipe, its write() blocks until someone reads; but the only
 * reader is blocked in wait() until the child exits. Deadlock. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void die(const char *msg) { perror(msg); exit(1); }

int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: %s N\n", argv[0]); return 2; }

    int fd[2];
    if (pipe(fd) == -1) die("pipe");

    pid_t pid = fork();
    if (pid == -1) die("fork");
    if (pid == 0) {                             /* child: stdout -> pipe, run the worker */
        if (dup2(fd[1], 1) == -1) die("dup2");
        close(fd[0]);
        close(fd[1]);
        execl("./04-worker", "04-worker", argv[1], (char *)NULL);
        die("execl ./04-worker");
    }

    close(fd[1]);                               /* parent: only reads */

    int status;
    fprintf(stderr, "parent: calling wait() before reading the pipe ... "
                    "(hangs if N > pipe capacity; Ctrl-C, or run under timeout)\n");
    if (wait(&status) == -1) die("wait");

    long captured = 0;
    char buf[4096];
    ssize_t n;
    while ((n = read(fd[0], buf, sizeof buf)) > 0)
        captured += n;
    if (n == -1) die("read");

    printf("parent: child exited with status %d, captured %ld bytes\n",
           WIFEXITED(status) ? WEXITSTATUS(status) : -1, captured);
    return 0;
}
