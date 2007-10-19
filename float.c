#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <machine/float.h>


double
doublex(char *x)
{
  int i;
  union { double d; unsigned char c[8]; } y;

  for (i = 0; i < 8; i++) {
    if (isalpha(*x)) {
    } else {
      
    }
  }
  return y.d;
}

void
bprintf(unsigned char c, int bits, int force)
{
  int i, zeroes;

  zeroes = 0;

  for (i = 8-bits; i < 8; i++) {
    if (c&(0x80>>i)) {
      while (zeroes--) {
	printf("0");
      }
      zeroes = 0;
      printf("1");
    } else {
      zeroes++;
    }
  }

  if (force && zeroes) {
    while (zeroes--) {
      printf("0");
    }
  }
}

void
xprintf(unsigned char c, int nibbles, int force)
{
  if (c || force) {
    printf("%0*x", nibbles, c);
  }
}


int
anyon(unsigned char *p, int num)
{
  while (num--) {
    if (*p++) {
      return 1;
    }
  }
  return 0;
}

#define BCNSIZE  (DBL_MANT_DIG/4)
#define BCNLAST  (BCNSIZE-1)
void
char2bcn(unsigned char *p, unsigned char bcn[BCNSIZE])
{
  int i, j;

  i = BCNLAST;

  for (j = 0; j <= 5; j++) {
    bcn[i--] = p[j]&0x0f;
    bcn[i--] = (p[j]&0xf0)>>4;
  }

  bcn[i] = p[6]&0x0f;
}


int
bcnbit(unsigned char bcn[BCNSIZE], int bit)
{
  return (bcn[bit/4]&(0x8>> (bit%4))) != 0;
}

#define BCDSIZE DBL_MANT_DIG
#define BCDLAST (BCDSIZE-1)

void
bcdfixup(unsigned char bcd[BCDSIZE])
{
  int j, tmp;

  j = BCDLAST;

  while (j >= 0) {
    if (bcd[j] >= 10) {
      if (j <= 1) {
	abort();
      }
      tmp = bcd[j];
      bcd[j] = tmp - ((tmp/10)*10);
      bcd[j-1] += (tmp-bcd[j])/10;
    }
    j--;
  }
}

void
bcn2bcd(unsigned char bcn[BCNSIZE], unsigned char bcd[BCDSIZE])
{
  int i, j;

  for (i = BCNLAST, j = BCDLAST; i >= 0; i--, j--) {
    bcd[j] = bcn[i];
  }
}
    


void
bcdadd(unsigned char bcd[BCDSIZE], int addend)
{
  bcd[BCDLAST] += addend;
  bcdfixup(bcd);
}

void
bcdmult(unsigned char bcd[BCDSIZE], int multiplicand)
{
  int i;

  for (i = 0; i <= BCDLAST; i++) {
    bcd[i] *= multiplicand;
  }
  bcdfixup(bcd);
}

void
bcdprint(unsigned char bcd[BCDSIZE])
{
  int i, force = 0;

  for (i = 0; i <= BCDLAST; i++) {
    if (bcd[i] || force) {
      printf("%d", bcd[i]);
      force = 1;
    }
  }
  if (force == 0) {
    printf("0");
  }
}
void
xdouble(double x)
{
  unsigned char *p = (unsigned char *)&x;
  int sign, exp, i, zeroes;

  if ((p[7]&0x80) != 0) {
    sign = 1;
  } else {
    sign = 0;
  }

  exp = ((p[7]&0x7f)<<4) | ((p[6]&0xf0)>>4);
  exp -= 0x3ff;

  printf("%c/%d/0x1.", sign? '-' : '+', exp);

  printf("%1x", p[6]&0x0f);
  zeroes = 0;
  for (i = 5; i >= 0; i--) {
    if (p[i]&0xf0) {
      while (zeroes--) {
	printf("0");
      }
      zeroes = 0;
      printf("%1x", (p[i]&0xf0)>>4);
    } else {
      zeroes++;
    }
    if (p[i]&0x0f) {
      while (zeroes--) {
	printf("0");
      }
      zeroes = 0;
      printf("%1x", p[i]&0x0f);
    } else {
      zeroes++;
    }
  }
}


void
sdouble(double x)
{
  unsigned char *p = (char *)&x;
  int sign, exp, i, j;
  unsigned char bcd[BCDSIZE], mantissa[BCNSIZE];

  if ((p[7]&0x80) != 0) {
    sign = 1;
  } else {
    sign = 0;
  }

  exp = ((p[7]&0x7f)<<4) | ((p[6]&0xf0)>>4);
  exp -= 0x3ff;

  char2bcn(p, mantissa);

  bzero(bcd, sizeof bcd);
  if (exp > 0) {
    bcdadd(bcd, 1);
  }


  i = exp;
  j = 0;
  while (i > 0) {
    bcdmult(bcd, 2);
    if (bcnbit(mantissa, j)) {
      bcdadd(bcd, 1);
    }
    j++;
    i--;
  }

  bcdprint(bcd);
  printf("\n");

  printf("%s", sign? "-" : "");
  printf("1.");
  bprintf(p[6]&0x0f, 4, anyon(p, 6));
  bprintf(p[5], 8, anyon(p, 5)); bprintf(p[4], 8, anyon(p, 4));
  bprintf(p[3], 8, anyon(p, 3)); bprintf(p[2], 8, anyon(p, 2));
  bprintf(p[1], 8, anyon(p, 1)); bprintf(p[0], 8, 0);
  printf(" ");
  printf("1.");
  xprintf(p[6]&0x0f, 1, anyon(p, 6));
  xprintf(p[5], 2, anyon(p, 5)); xprintf(p[4], 2, anyon(p, 4));
  xprintf(p[3], 2, anyon(p, 3)); xprintf(p[2], 2, anyon(p, 2));
  xprintf(p[1], 2, anyon(p, 1)); xprintf(p[0], 2, 0);
  printf("*2^%d", exp);
}


int
main(int argc, char *argv[])
{
  double x;
  char str[20];

  while (!feof(stdin)) {
    if (fgets(str, sizeof str, stdin) == NULL) {
      return 0;
    }
    x = strtod(str, 0);
    xdouble(x);
    printf(" for %e\n", x);
    xdouble(1.0-x);
    printf(" for %e\n", 1.0-x);
    sdouble(x);
    printf(" for %e\n", x);
  }
  return 0;
}
