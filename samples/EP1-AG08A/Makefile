TARGETS = read_analog

CC = gcc
AR = ar
CXXFLAGS = -O2 -Wall
ARFLAGS = rv

INCDIR = -I../../module

OBJS = $(SRCS:%=%.o)

.PHONY: all clean
all: $(TARGETS)

read_analog: read_analog.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

.c.o:
	$(CC) $(CXXFLAGS) -c $< $(INCDIR) $(LIBDIR) $(LIBS)

clean:
	$(RM) $(TARGET) $(OBJS) *~
