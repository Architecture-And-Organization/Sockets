CXXFLAGS+=-Wall -Werror -std=c++11

PROGS=client server server2 client2 client3 server3 client4 server4 client5 server5

all: $(PROGS)

clean:
	rm -f $(PROGS)

world: clean all
