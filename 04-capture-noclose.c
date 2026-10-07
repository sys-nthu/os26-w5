/* Part 3: the parent that never sees end of file. This is 04-capture.c with
 * ONE line removed: the parent no longer closes its copy of fd[1]. The read
 * loop drains the worker's output and then waits for EOF that cannot come,
 * because the parent itself still holds a write end. Try `diff`. */
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

    /* (nothing here: the parent keeps fd[1] open) */

    long captured = 0;                          /* read to EOF first ... */
    char buf[4096];
    ssize_t n;
    while ((n = read(fd[0], buf, sizeof buf)) > 0)
        captured += n;
    if (n == -1) die("read");

    int status;                                 /* ... then collect the exit status */
    if (wait(&status) == -1) die("wait");

    printf("parent: child exited with status %d, captured %ld bytes\n",
           WIFEXITED(status) ? WEXITSTATUS(status) : -1, captured);
    return 0;
}
