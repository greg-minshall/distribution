#include <sys/time.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define  NUM(a)  (sizeof (a)/(sizeof (a)[0]))
#define  LAST(a) (&((a)[NUM(a)-1]))

#define  pi() M_PI


typedef int (*distproc_t)(int argc, char *argv[]);


int exponential(int argc, char *argv[]);
int geometric(int argc, char *argv[]);
int normal(int argc, char *argv[]);
int pareto(int argc, char *argv[]);
int poisson(int argc, char *argv[]);

struct distributions {
  distproc_t proc;
  char *distname;
  char *usage;
} distributions[] = {
  { exponential, "exponential", "[MEAN (1.0)]" },
  { geometric, "geometric", "[PROBABILITY (0.5)]" },
  { normal, "normal", "[MEAN (0.0)] [STDDEV (1.0)" },
  { pareto, "pareto", "[LOCATION (1.0)] [SHAPE (1.0)]" },
  { poisson, "poisson", "[MEAN (1.0)]" },
};

int iterations;			/* number of iterations */
char *cmdname,			/* name of command */
     *distname;			/* name of distribution invoked */

/********************
 * Utility routines *
 ********************/

double
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


int
exponential(int argc, char *argv[])
{
  double ans, b;

  b = 1.0;

  if (argc > 0) {
    b = atof(argv[0]);
    if (b < 0) {
      fprintf(stderr, "invalid mean %g; must be >= 0\n", b);
      return 1;
    }
  }

  if (argc > 1) {
    fprintf(stderr, "too many arguments to exponential\n");
    return 1;
  }

  while (iterations--) {
    ans = - ( b * log(unitrectangular()) );
    printf("%g\n", ans);
  }

  return 0;
}

int
geometric(int argc, char *argv[])
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


int
normal(int argc, char *argv[])
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
					      
    
int
pareto(int argc, char *argv[])
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


int
poisson(int argc, char *argv[])
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



/**********************
 * Overhead routines  *
 **********************/

static void
help(void)
{
  struct distributions *dist;

  fprintf(stderr, "Distributions:\n");

  for (dist = distributions; dist <= LAST(distributions); dist++) {
    fprintf(stderr, "\t%s %s\n", dist->distname, dist->usage);
  }
  exit(1);
}


static void
usage(void)
{
  fprintf(stderr,
	  "usage: %s [-h] [-i ITERATIONS] [-s SEED] DISTRIBUTION PARAMETERS\n",
	  cmdname);
  exit(1);
}

int
main(int argc, char *argv[])
{
  int opt;
  int sflag;
  unsigned soption;
  struct distributions *dist;
  struct timeval tv;
  extern char *optarg;
  extern int optind, optreset;

  cmdname = argv[0];

  sflag = 0;
  iterations = 1;

  optreset = 1;
  optind = 1;

  while ((opt = getopt(argc, argv, "hi:s:")) != EOF) {
    switch (opt) {
    case 'h':
      help();
      /*NOTREACHED*/
      break;
    case 'i':
      iterations = atoi(optarg);
      break;
    case 's':
      soption = atoi(optarg);
      sflag = 1;
      break;
    default:
      usage();
      /*NOTREACHED*/
    }
  }

  argc -= optind;
  argv += optind;

	    
	
  if (argc == 0) {
    usage();
    /*NOTREACHED*/
  }

  /* initialize random number generator */
  if (sflag) {
    (void)srandom(soption);
  } else {
    gettimeofday(&tv, 0);
    (void)srandom(tv.tv_usec ^ (tv.tv_usec>>16));
  }

  distname = argv[0];
  argc--;
  argv++;

  for (dist = distributions; dist <= LAST(distributions); dist++) {
    if (!strcmp(distname, dist->distname)) {
      return dist->proc(argc, argv);
    }
  }

  fprintf(stderr, "unknown distribution \"%s\"\n", distname);
  usage();
  /*NOTREACHED*/
  return 0;
}
