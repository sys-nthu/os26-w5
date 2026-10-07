/* Part 3: how many bytes can sit in a pipe before write() blocks? */
#define _GNU_SOURCE                             /* F_GETPIPE_SZ is Linux-only */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void die(const char *msg) { perror(msg); exit(1); }

int main(void)
{
    int fd[2];
    if (pipe(fd) == -1) die("pipe");

    int cap = fcntl(fd[0], F_GETPIPE_SZ);
    if (cap == -1) die("fcntl(F_GETPIPE_SZ)");
    printf("pipe capacity on this machine: %d bytes\n", cap);
    return 0;
}
