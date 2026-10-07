/* Part 2: read integers, one per line, from stdin and print their sum.
 * stdin is a data channel: a line that is not an integer (for example a
 * debugging message that leaked into the pipe) is an error, not something to skip. */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char line[256];
    long total = 0;

    while (fgets(line, sizeof line, stdin) != NULL) {
        line[strcspn(line, "\n")] = '\0';       /* drop the trailing newline */

        char *end;
        errno = 0;
        long v = strtol(line, &end, 10);
        if (line[0] == '\0' || *end != '\0' || errno != 0) {
            fprintf(stderr, "sum: not a number: \"%s\"\n", line);
            return 1;
        }
        total += v;
    }

    printf("sum = %ld\n", total);
    return 0;
}
