/* Part 2: write "1\n2\n...N\n" to fd 1 (stdout) with write().
 * With DEBUG set in the environment, print a debugging message first.
 * Where that line goes, fd 1 or fd 2, decides whether `./02-sum`
 * downstream can still parse what it receives. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void die(const char *msg) { perror(msg); exit(1); }

/* write() the whole buffer, retrying after a short write */
static void write_all(int fd, const char *buf, size_t len)
{
    while (len > 0) {
        ssize_t n = write(fd, buf, len);
        if (n == -1) die("write");
        buf += n;
        len -= (size_t)n;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: %s N\n", argv[0]); return 2; }
    long n = strtol(argv[1], NULL, 10);

    if (getenv("DEBUG")) {
        char msg[64];
        int len = snprintf(msg, sizeof msg, "numbers: starting (N=%ld)\n", n);
        write_all(1, msg, (size_t)len);         /* debugging message -> fd 1, the data channel */
        /* write_all(2, msg, (size_t)len); */   /* Part 2: move the debugging message here (fd 2) */
    }

    for (long i = 1; i <= n; i++) {
        char line[32];
        int len = snprintf(line, sizeof line, "%ld\n", i);
        write_all(1, line, (size_t)len);
    }
    return 0;
}
