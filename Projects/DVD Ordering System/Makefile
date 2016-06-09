CC=gcc
CCOPTS=-g

TARGETS=ex1_server ex1_client ex2_server ex2_client

all: $(TARGETS)

ex1_server: ex1_server.c
	$(CC) $(CCOPTS) -o $@ $^

ex1_client: ex1_client.c
	$(CC) $(CCOPTS) -o $@ $^

ex2_server: ex2_server.c
	$(CC) $(CCOPTS) -o $@ $^

ex2_client: ex2_client.c
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -f $(TARGETS) *.o
