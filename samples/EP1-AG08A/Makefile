TARGETS = read_analog

CC = gcc
AR = ar
CXXFLAGS = -O2 -Wall
ARFLAGS = rv

INCDIR = -I../../module

SRCS = read_analog.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean
all: $(TARGETS)

read_analog: read_analog.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

.c.o:
	$(CC) $(CXXFLAGS) -c $< $(INCDIR) $(LIBDIR) $(LIBS)

clean:
	$(RM) $(TARGETS) $(OBJS) *~
