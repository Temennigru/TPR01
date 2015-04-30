CC=g++
CLIENTSOURCES=client_core.cpp
SERVERSOURCES=server_core.cpp
CLIENTOBJS:=$(CLIENTSOURCES:.cpp=.o)
SERVEROBJS:=$(SERVERSOURCES:.cpp=.o)
CFLAGS=-DNDEBUG

.PHONY: clean all set_debug

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

all: server client

client: $(CLIENTOBJS) client.o
	$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

server: $(SERVEROBJS) server.o
	$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

client.o: main.cpp
	$(CC) main.cpp -c -o client.o $(CFLAGS) -D CLIENT

server.o: main.cpp
	$(CC) main.cpp -c -o client.o $(CFLAGS) -D SERVER

$(CLIENTOBJS): %.o : %.h

$(CLIENTOBJS): %.o : %.cpp

$(SERVEROBJS): %.o : %.h

$(SERVEROBJS): %.o : %.cpp

debug: | set_debug all

set_debug:
	$(eval CFLAGS=-Wall -DDEBUG)

clean:
	$(RM) $(CLIENTOBJS) $(SERVEROBJS) client.o server.o client server