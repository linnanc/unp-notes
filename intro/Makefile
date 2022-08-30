

CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
CC = gcc

objs-daytimetcpcli = daytimetcpcli.o ../lib/error.o ../lib/wrapsock.o  ../lib/wrapunix.o
objs-daytimetcpsrv = daytimetcpsrv.o ../lib/error.o ../lib/wrapsock.o  ../lib/wrapunix.o
objs-byteorder = byteorder.o ../lib/error.o ../lib/wrapsock.o  ../lib/wrapunix.o

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

daytimetcpcli: $(objs-daytimetcpcli)
	${CC} ${CFLAGS} -o $@ $^

daytimetcpsrv: $(objs-daytimetcpsrv)
	${CC} ${CFLAGS} -o $@ $^

byteorder: $(objs-byteorder)
	${CC} ${CFLAGS} -o $@ $^

clean:
	rm *.o
