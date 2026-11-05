CC = g++
CFLAGS = -Wall -Wextra -g -std=c++11
TARGET = daytime_client
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
