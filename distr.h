#ifndef __distr_h__
#define __distr_h__

/* called ``synchronously'' during distr() to return value(s) */
typedef void (*distrcb_t)(void *cookie, double value);

/*
 * create a new distribution, starting from a given seed;
 * returns a distribution context
 *
 * returns 0 if the name doesn't match (or, another error such
 * as no memory).
 */
void *distrnew(char *name, unsigned long seed);

/*
 * run ``iterations'' of a distribution with ``context'',
 * calling ``valuecb'' with ``cookie'' for each iteration.
 */
int distr(void *context, int iterations,
           distrcb_t valuecb, void *cookie, int argc, char *argv[]);

/* print out help for distributions */
void distrhelp(void);

#endif /* ndef __distr_h__ */
