#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "distr.h"

static void
usage(char *cmdname)
{
    fprintf(stderr,
         "usage: %s [-h] [-i ITERATIONS] [-s SEED] DISTRIBUTION PARAMETERS\n",
            cmdname);
    exit(1);
}


static void printit(void *cookie, double value) {
    printf("%g\n", value);
}


int
main(int argc, char *argv[])
{
    int opt;
    int sflag;
    unsigned long seed;
    int iterations;
    struct timeval tv;
    extern char *optarg;
    extern int optind;
    char *cmdname;			/* name of command */
    char *distname;			/* name of distribution invoked */
    void *x;

    cmdname = argv[0];

    sflag = 0;

    optind = 1;
    iterations = 1;

    while ((opt = getopt(argc, argv, "hi:s:")) != EOF) {
        switch (opt) {
        case 'h':
            distrhelp();
            exit(1);
            break;
        case 'i':
            iterations = atoi(optarg);
            break;
        case 's':
            seed = atoi(optarg);
            sflag = 1;
            break;
        default:
            usage(cmdname);
            /*NOTREACHED*/
        }
    }

    argc -= optind;
    argv += optind;

    if (argc == 0) {
        usage(cmdname);
        /*NOTREACHED*/
    }

    /* initialize random number generator if we haven't already */
    if (sflag == 0) {
        gettimeofday(&tv, 0);
        seed = tv.tv_usec ^ (tv.tv_usec>>16);
    }

    distname = argv[0];

    x = distrnew(argv[0], seed);
    if (x == 0) {
        fprintf(stderr, "unknown distribution \"%s\" (or out of memory)\n",
                distname);
        usage(cmdname);
        /*NOTREACHED*/
        return 0;
    }

    return distr(x, iterations, printit, 0, argc, argv);
}
