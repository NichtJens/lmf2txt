TARGET = lmf2txt
OBJECTS = LMF_IO.o
HEADERS = $(patsubst %.o, %.h, $(OBJECTS))
CC = g++
LIBS = #-lm
#CFLAGS = -g -O0 #for debug symbols
CFLAGS = -O2 #for optimization
CFLAGS += -Wall -Werror


all: $(TARGET)

$(TARGET): $(OBJECTS) $(TARGET).o
	$(CC) $? $(CFLAGS) $(LIBS) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(TARGET) $(OBJECTS) $(TARGET).o

.PHONY: all clean



