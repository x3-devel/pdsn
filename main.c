#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include <pdsn.h>

const char *pdsn_format = "bash";

void print_usage()
{
    printf("Usage: pdsn [-f [bash|export|yaml]] schema://[username[:password]@]hostname[:port][/path] ...\n");
}

int main(int argc, char **argv) {

    if (argc < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    int opt;
    while((opt = getopt(argc, argv, ":f:")) != -1) 
    { 
        switch(opt) 
        { 
            case 'f':
                pdsn_format = optarg;
                break;
            case ':': 
                printf("option needs a value\n"); 
                break; 
            case '?': 
                printf("unknown option: %c\n", optopt);
                break; 
        } 
    }

    // no positional arguments
    if (optind > argc) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // execution
    const char *input_dsn = "none";

    for (; optind < argc; optind++) {

        input_dsn = argv[optind];
        int format = pdsn_get_format(pdsn_format);

        dsn_t dsn = { 0 };
        int ret = parse_dsn(input_dsn, &dsn);
        if (ret) {
            perror("parse dsn failed");
        }

        char buf[1024] = { 0 };
        dsn_format(format, NULL, &dsn, buf, sizeof buf);

        fprintf(stderr, "Format: %s\n", pdsn_format);
        printf("%s%s", buf, (optind == (argc-1)) ? "" : "\n"); // do not print '\n' for last given

        dsn_destroy(&dsn);
    }

    return 0;
}
