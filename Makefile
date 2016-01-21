TARGET = lmf2txt
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)
CC = g++
LIBS = #-lm
#CFLAGS = -g -O0 #for debug symbols
CFLAGS = -O2 #for optimization
CFLAGS += -Wall -Werror



.PHONY: all clean


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(TARGET) $(OBJECTS)



