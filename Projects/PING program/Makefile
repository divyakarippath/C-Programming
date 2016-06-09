

OBJS = main.o proc_v4.o readloop.o \
		send_v4.o tv_sub.o -lpthread
PROGS =	ping

all:	${PROGS}

ping:	${OBJS}
		${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
