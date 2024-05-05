CC = gcc
ARGS = -Os -s
LARGS =

target: virus host victim1 victim2 victim3 host1 host2 host3

virus: virus.c
	$(CC) $(ARGS) -o $@ $^ $(LARGS)

host: host.o
	$(CC) $(ARGS) -o $@ $^ $(LARGS)

host1: host.o
	$(CC) $(ARGS) -o $@ $^ $(LARGS)

host2: host.o
	$(CC) $(ARGS) -o $@ $^ $(LARGS)

host3: host.o
	$(CC) $(ARGS) -o $@ $^ $(LARGS)

host.o: host.c
	$(CC) $(ARGS) -c $^ -o $@ $(LARGS)

victim1:
	cp /bin/ls ./victim1
	chmod 664 ./victim1

victim2:
	cp /usr/bin/wc ./victim2
	chmod 664 ./victim2

victim3:
	cp /usr/bin/file ./victim3
	chmod 664 ./victim3

.PHONY: clean

clean:
	rm -f main virus host host1 host2 host3 victim1 victim2 victim3 out *.o *.asm seed
