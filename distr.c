#include <sys/time.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


int iterations;			/* number of iterations */
char *cmdname,			/* name of command */
     *distname;			/* name of distribution invoked */


int
exponential(int argc, char *argv[])
{
  double r, ans, b;
  long lr;

  b = 1.0;

  if (argc > 0) {
    b = atof(argv[0]);
  }

  while (iterations--) {
    lr = random();
    if (lr) {
      r = (lr*1.0)/(0x7fffffff-1.0);
    }
    ans = - ( b * log(r) );
    printf("%g\n", ans);
  }

  return 0;
}

static void
help(void)
{
  fprintf(stderr, "Distributions:\n");
  fprintf(stderr, "\texponential [MEAN (1)]\n");
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

  if (!strcmp(distname, "exponential")) {
    return exponential(argc, argv);
  } else {
    fprintf(stderr, "unknown distribution \"%s\"\n", distname);
    usage();
    /*NOTREACHED*/
  }
  return 0;
}
