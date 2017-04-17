.SUFFIXES: .c .o
CC = gcc
EXEC = dnslookup
CCFLAGS = -g -pedantic -Wall -Wextra
# OBJS = portmapper.o dnsresolver.o
OBJS = dnsresolver.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${OBJS}

# portmapper.o: portmapper.c
dnsresolver.o: dnsresolver.c
