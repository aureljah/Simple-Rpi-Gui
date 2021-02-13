CXX = g++

RM = rm -rf

NAME = server

LDFLAGS = -L. -L./ -L/usr/lib/x86_64-linux-gnu -lssl -lpthread

CFLAGS = -Wall -O3

SRC = src/main.cpp \
		../shared_src/socket/linux/Socket.cpp \
		../shared_src/socket/OpensslWrapper.cpp

SRC_TEST = src/serv_test.cpp \
		../shared_src/socket/linux/Socket.cpp \
		../shared_src/socket/OpensslWrapper.cpp

HDR =	-I../shared_src/socket \
		-I../shared_src/socket/linux

all: build

build:
	$(CXX) $(CFLAGS) $(SRC) $(HDR) -o $(NAME) $(LDFLAGS)

debug:
	$(CXX) $(CFLAGS) -g $(SRC) $(HDR) -o $(NAME) $(LDFLAGS)

clean:
	$(RM) $(NAME)

test: 
	$(RM) test
	$(CXX) $(CFLAGS) $(SRC_TEST) $(HDR) -o test $(LDFLAGS)

fclean: clean

re: clean all

.PHONY: all re clean fclean test
