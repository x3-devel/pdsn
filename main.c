#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include <pdsn.h>

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage: pdsn schema://[username[:password]@]hostname[:port][/path]\n");
        exit(EXIT_FAILURE);
    }

    const char *input = argv[1];
    dsn_t dsn = { 0 };

    int ret = parse_dsn(input, &dsn);
    if (ret) {
        perror("parse dsn failed");
    }

    char buf[1024] = { 0 };
    const char *format_name = NULL;
    dsn_format(PDSN_FMT_BASH, &format_name, &dsn, buf, sizeof buf);

    fprintf(stderr, "Format: %s\n", format_name);
    printf("%s", buf);

    dsn_destroy(&dsn);

    return 0;
}
