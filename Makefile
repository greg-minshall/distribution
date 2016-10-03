CFLAGS = -g -Wall
LDFLAGS = -lm

OBJS = distr.o distrmain.o distrtcl.o
BINARIES = distribution distrtcl

distribution: distr.o distrmain.o
	${CC} ${CFLAGS} distr.o distrmain.o ${LDFLAGS} -o distribution

distrtcl: distr.o distrtcl.o
	${CC} ${CFLAGS} ${LDFLAGS} distr.o distrtcl.o \
				-o distribution $+ -ltcl -lm

clean:
	rm -f ${OBJS} ${BINARIES} distribution distribution.core
