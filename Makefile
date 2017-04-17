EXEC = dnslookup portmapper dnsresolver

all: dnslookup portmapper dnsresolver

clean:
	rm -f ${EXEC}
