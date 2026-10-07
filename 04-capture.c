/* Part 3: capture a child's output through a pipe, the RIGHT way round.
 * The parent drains the pipe until read() returns 0 (the child has exited
 * and closed its end), and only then calls wait(). The child is never
 * stuck in write() with nobody reading. */
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

    close(fd[1]);                               /* parent: only reads. REMOVE THIS LINE AND THE PROGRAM NEVER FINISHES */

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
