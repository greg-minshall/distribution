#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "distr.h"

#define  NUM(a)  (sizeof (a)/(sizeof (a)[0]))
#define  LAST(a) (&((a)[NUM(a)-1]))

#define  pi() M_PI

#define RANDSTATE_NBYTES        256 /* number of bytes in initstate(3) state */

typedef struct distr distr_t, *distr_p;

typedef int (*distproc_t)(distr_p distr,
                          int iterations, distrcb_t valuecb, void *cookie,
                          int argc, char *argv[]);


static int exponential(distr_p distr,
                       int iterations, distrcb_t valuecb, void *cookie,
                       int argc, char *argv[]);
static int geometric(distr_p distr,
                     int iterations, distrcb_t valuecb, void *cookie,
                     int argc, char *argv[]);
static int normal(distr_p distr,
                  int iterations, distrcb_t valuecb, void *cookie,
                  int argc, char *argv[]);
static int pareto(distr_p distr,
                  int iterations, distrcb_t valuecb, void *cookie,
                  int argc, char *argv[]);
static int poisson(distr_p distr,
                   int iterations, distrcb_t valuecb, void *cookie,
                   int argc, char *argv[]);
static int uniform(distr_p distr,
                   int iterations, distrcb_t valuecb, void *cookie,
                   int argc, char *argv[]);

/* mapping name to distribution functions */
static struct distributions {
  distproc_t proc;
  char *distname;
  char *usage;
} distributions[] = {
  { exponential, "exponential", "[MEAN (1.0)]" },
  { geometric, "geometric", "[PROBABILITY (0.5)]" },
  { normal, "normal", "[MEAN (0.0)] [STDDEV (1.0)" },
  { pareto, "pareto", "[LOCATION (1.0)] [SHAPE (1.0)]" },
  { poisson, "poisson", "[MEAN (1.0)]" },
  { uniform, "uniform", "[LOWER (0) UPPER (1)]" },
};

/* describes a given distribution instance.  there may be multiple
 * instances of the same distribution name ("geometric", say); each
 * has its own random number space feeding it, so should be independent.
 */

struct distr {
    distproc_t proc;
    char randstate[RANDSTATE_NBYTES];
};


/********************
 * Utility routines *
 ********************/


static int
getdouble(char *string, double *result) {
    char *endptr;

    errno = 0;

    *result = strtod(string, &endptr);

    if ((*result == 0) && (errno == ERANGE)) {
        return 1;
    }

    if ((*result == HUGE_VAL) && (errno == ERANGE)) {
        return 1;
    }

    if (*endptr == 0) {
        return 0;
    } else if (isspace((int)*endptr)) {
        return 0;
    } else {
        return EINVAL;
    }
}


static double
unitrectangular(void)
{
    long lr;
    double r;

    lr = random();
    if (lr) {
        r = (lr*1.0)/(0x7fffffff-1.0);
    } else {
        r = 0.0;
    }
    return r;
}


/*****************
 * Distributions *
 *****************/


static int exponential(distr_p distr,
                       int iterations, distrcb_t valuecb, void *cookie,
                       int argc, char *argv[])
{
    double ans, b;

    b = 1.0;

    if (argc > 1) {
        if (getdouble(argv[1], &b)) {
            fprintf(stderr,
                  "%s: invalid mean ``%s''; must be a floating point number\n",
                    argv[0], argv[1]);
            return 1;
        }
        if (b < 0) {
            fprintf(stderr, "%s: invalid mean %g; must be >= 0\n",
                    argv[0], b);
            return 1;
        }
    }

    if (argc > 2) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
        return 1;
    }

    while (iterations--) {
        ans = - ( b * log(unitrectangular()) );
        printf("%g\n", ans);
    }

    return 0;
}

static int geometric(distr_p distr,
                     int iterations, distrcb_t valuecb, void *cookie,
                     int argc, char *argv[])
{
    double ans, p, log1mp;

    p = 0.5;

    if (argc > 0) {
        p = atof(argv[0]);
        if ((p < 0) || (p > 1.0)) {
            fprintf(stderr, "invalid value %g for p; must be 1.0 >= p >= 0\n", p);
            return 1;
        }
    }

    if (argc > 1) {
        fprintf(stderr, "too many parameters to geometric\n");
        return 1;
    }

    log1mp = log(1-p);

    while (iterations--) {
        ans = (log(unitrectangular())/log1mp)-1;
        ans = ceil(ans);
        printf("%g\n", ans);
    }

    return 0;
}


static int normal(distr_p distr,
                  int iterations, distrcb_t valuecb, void *cookie,
                  int argc, char *argv[])
{
    double mean, stddev, ans;

    mean = 0.0;
    stddev = 1.0;

    if (argc) {
        mean = atof(argv[0]);
        if (argc > 1) {
            stddev = atof(argv[1]);
            if (stddev <= 0) {
                fprintf(stderr, "invalid standard deviation %g; must be > 0\n",
                        stddev);
            }
            if (argc > 2) {
                fprintf(stderr, "too many parameters to normal\n");
                return 1;
            }
        }
    }

    while (iterations--) {
        ans = sqrt(-2*log(unitrectangular())) * sin(2*pi()*unitrectangular());
        ans = (stddev * ans) + mean;
        printf("%g\n", ans);
    }

    return 0;
}
					      
    
static int pareto(distr_p distr,
                  int iterations, distrcb_t valuecb, void *cookie,
                  int argc, char *argv[])
{
    double a, c, crecip, ans;

    a = 1;
    c = 1;

    if (argc) {
        a = atof(argv[0]);
        if (a <= 0) {
            fprintf(stderr, "invalid location %g; must be > 0\n", a);
            return 1;
        }
        if (argc > 1) {
            c = atof(argv[1]);
            if (c <= 0) {
                fprintf(stderr, "invalid shape %g; must be > 0\n", c);
                return 1;
            }
            if (argc > 2) {
                fprintf(stderr, "too many parameters for pareto\n");
                return 1;
            }
        }
    }

    crecip = 1/c;

    while (iterations--) {
        ans = a * pow(1.0-unitrectangular(), -crecip);
        printf("%g\n", ans);
    }

    return 0;
}


static int poisson(distr_p distr,
                   int iterations, distrcb_t valuecb, void *cookie,
                   int argc, char *argv[])
{
    double lambda, r, partial, sum;
    int i;

    lambda = 1.0;

    if (argc) {
        lambda = atof(argv[0]);
        if (lambda <= 0) {
            fprintf(stderr, "invalid mean %g; must be > 0\n", lambda);
            return 1;
        }
        if (argc > 1) {
            fprintf(stderr, "too many parameters for poisson\n");
            return 1;
        }
    }

    while (iterations--) {
        r = unitrectangular();
        partial = exp(-lambda);
        sum = partial;
        if (r < sum) {
            printf("0\n");
            continue;
        }
        i = 0;
        while (1) {
            i++;
            partial = (lambda * partial) / i;
            sum = sum+partial;
            if (r < sum) {
                printf("%d\n", i);
                break;
            }
        }
    }
    return 0;
}


static int uniform(distr_p distr,
                   int iterations, distrcb_t valuecb, void *cookie,
                   int argc, char *argv[])
{
    double a, b, len, ans;

    a = 0.0;
    b = 1.0;

    if (argc > 0) {
        if (argc != 2) {
            fprintf(stderr,
                    "uniform must have no parameters or exactly two parameters\n");
            return 1;
        }
        a = atof(argv[0]);
        b = atof(argv[1]);
        if (b <= a) {
            fprintf(stderr, "invalid LOWER %g UPPER %g; must be LOWER < UPPER\n",
                    a, b);
            return 1;
        }
    }

    len = b-a;
    while (iterations--) {
        ans = a + (len*unitrectangular());
        printf("%g\n", ans);
    }

    return 0;
}



static distr_p newdistr(void) {
    distr_p distr;

    distr = (distr_p) malloc(sizeof *distr);
    if (distr == 0) {
        return 0;
    }
    bzero(distr, sizeof *distr);

    return distr;
}


void
distrhelp(void)
{
    struct distributions *dist;

    fprintf(stderr, "Distributions:\n");

    for (dist = distributions; dist <= LAST(distributions); dist++) {
        fprintf(stderr, "\t%s %s\n", dist->distname, dist->usage);
    }
}


void *
distrnew(char *name, unsigned long seed) {
    distr_p distr;
    int i;

    for (i = 0; i < NUM(distributions); i++) {
        if (strcmp(distributions[i].distname, name) == 0) {
            distr = newdistr();
            if (distr == 0) {
                return 0;
            }
            distr->proc = distributions[i].proc;
            initstate(seed, distr->randstate, sizeof distr->randstate);
            return distr;
        }
    }
    /* didn't find it */
    return 0;
}


int distr(void *context, int iterations,
           distrcb_t valuecb, void *cookie, int argc, char *argv[]) {
    distr_p distr = (distr_p)context;

    (void) setstate(distr->randstate);
    return distr->proc(distr, iterations, valuecb, cookie, argc, argv);
}
