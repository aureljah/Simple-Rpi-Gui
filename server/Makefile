CXX = g++

RM = rm -rf

# When edited this file, use: https://texttoolkit.com/compilation-database-generator to make a new compile_commands.json (for vscode)

NAME = server

LDFLAGS = -L. -L./ -L/usr/lib/x86_64-linux-gnu -lssl -lpthread

CFLAGS = -Wall -O3

RPI_SRC = src/gpio/rpi/GpioWrapper.cpp
RPI_HDR = -I./src/gpio/rpi

TEST_SRC = src/gpio/test/GpioWrapper.cpp
TEST_HDR = -I./src/gpio/test

SRC = src/main.cpp \
		../shared_src/socket/linux/Socket.cpp \
		../shared_src/socket/OpensslWrapper.cpp \
		src/MainServer.cpp \
		src/live/LiveMode.cpp \
		src/live/LivePin.cpp \

HDR =	-I../shared_src/socket \
		-I../shared_src/socket/linux

all: test_build

test_build:
	$(CXX) $(CFLAGS) $(SRC) $(TEST_SRC) $(HDR) $(TEST_HDR) -o $(NAME) $(LDFLAGS)

debug:
	$(CXX) $(CFLAGS) -g $(SRC) $(TEST_SRC) $(HDR) $(TEST_HDR) -o $(NAME) $(LDFLAGS)

rpi:
	$(CXX) $(CFLAGS) $(SRC) $(RPI_SRC) $(HDR) $(RPI_HDR) -o $(NAME) $(LDFLAGS)

clean:
	$(RM) $(NAME)

fclean: clean

re: clean all

.PHONY: all re clean fclean test
