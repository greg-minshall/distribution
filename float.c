#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>


void
xdouble(double x)
{
  int i;
  unsigned char *p = (unsigned char *)&x;

  for (i = 0; i < 8; i++) {
    printf("%02x", p[i]);
  }
}

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

void
sdouble(double x)
{
  unsigned char *p = (char *)&x;
  int sign, exp;

  if ((p[7]&0x80) != 0) {
    sign = 1;
  } else {
    sign = 0;
  }

  exp = ((p[7]&0x7f)<<4) | ((p[6]&0xf0)>>4);

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
  printf("*2^%d", exp - 0x3ff);
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
    x = atof(str);
    xdouble(x);
    printf(" ");
    sdouble(x);
    printf(" for %e\n", x);
  }
  return 0;
}
