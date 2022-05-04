TARGETS = lmf2txt WriteLMF
TARGET_OBJECTS = $(addsuffix .o, $(TARGETS))

PYBIND = lmf4py
OBJECTS = LMF_IO.o
HEADERS = $(patsubst %.o, %.h, $(OBJECTS))

CC = g++
LIBS = #-lm
#CFLAGS = -g -O0 #for debug symbols
CFLAGS = -O2 #for optimization
CFLAGS += -Wall -Werror


all: $(TARGETS)

$(TARGETS): %: $(OBJECTS) %.o
	$(CC) $? $(CFLAGS) $(LIBS) -o $@


python: CFLAGS += -fPIC -shared -I/usr/include/python2.7 -lpython2.7 -lboost_python -Wno-unused-function
python: $(PYBIND).so

$(PYBIND).so: $(OBJECTS) $(PYBIND).o
	$(CC) $? $(CFLAGS) $(LIBS) -o $@


%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(TARGET_OBJECTS) $(OBJECTS) $(PYBIND).o $(PYBIND).so

.PHONY: all python clean



