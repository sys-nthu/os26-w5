/* Part 1: `cat scores.csv | grep niko`, built by hand.
 * pipe() -> fork() -> dup2() -> execlp(), twice. The shell does exactly
 * these steps for you every time you type a `|`. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void die(const char *msg) { perror(msg); exit(1); }

int main(void)
{
    int fd[2];                                  /* fd[0]: read end, fd[1]: write end */
    if (pipe(fd) == -1) die("pipe");

    pid_t p1 = fork();
    if (p1 == -1) die("fork");
    if (p1 == 0) {                              /* child 1: cat scores.csv */
        if (dup2(fd[1], 1) == -1) die("dup2");  /* stdout now goes into the pipe */
        close(fd[0]);
        close(fd[1]);                           /* fd 1 still refers to the pipe */
        execlp("cat", "cat", "scores.csv", (char *)NULL);
        die("execlp cat");
    }

    pid_t p2 = fork();
    if (p2 == -1) die("fork");
    if (p2 == 0) {                              /* child 2: grep niko */
        if (dup2(fd[0], 0) == -1) die("dup2");  /* stdin now comes from the pipe */
        close(fd[0]);                           /* fd 0 still refers to the pipe */
        close(fd[1]);
        execlp("grep", "grep", "niko", (char *)NULL);
        die("execlp grep");
    }

    close(fd[0]);                               /* parent: hold no pipe ends, */
    close(fd[1]);                               /* or grep never sees EOF     */
    if (wait(NULL) == -1) die("wait");
    if (wait(NULL) == -1) die("wait");
    return 0;
}
