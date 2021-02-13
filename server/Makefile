CXX = g++

RM = rm -rf

NAME = server

LDFLAGS = -L. -L./ -L/usr/lib/x86_64-linux-gnu -lssl

CFLAGS = -Wall -O3

SRC = src/main.cpp \
		../shared_src/socket/linux/Socket.cpp \
		src/openssl_wrapper/OpensslWrapper.cpp

HDR =	-I../shared_src/socket \
		-I../shared_src/socket/linux \
		-Isrc/openssl_wrapper

all: build

build:
	$(CXX) $(CFLAGS) $(SRC) $(HDR) -o $(NAME) $(LDFLAGS)

debug:
	$(CXX) $(CFLAGS) -g $(SRC) $(HDR) -o $(NAME) $(LDFLAGS)

clean:
	$(RM) $(NAME)

fclean: clean

re: clean all

.PHONY: all re clean fclean
