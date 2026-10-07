/* Part 3: a child that writes N bytes of 'x' to stdout, standing in for any
 * chatty command whose output a parent wants to capture through a pipe. */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHUNK 32768

static void die(const char *msg) { perror(msg); exit(1); }

int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: %s N\n", argv[0]); return 2; }
    long total = strtol(argv[1], NULL, 10);

    /* If the reader disappears, write() fails with EPIPE instead of SIGPIPE
     * killing us silently, so we control what happens next. */
    signal(SIGPIPE, SIG_IGN);

    static char chunk[CHUNK];
    memset(chunk, 'x', sizeof chunk);

    long left = total;
    while (left > 0) {
        size_t want = left < CHUNK ? (size_t)left : CHUNK;
        ssize_t n = write(1, chunk, want);      /* may be a short write: loop */
        if (n == -1) {
            if (errno == EPIPE) return 1;       /* nobody is reading: leave quietly */
            die("write");
        }
        left -= n;
    }

    fprintf(stderr, "worker: wrote %ld bytes, exiting\n", total);
    return 0;
}
