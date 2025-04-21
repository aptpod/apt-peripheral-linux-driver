TARGETS = read_can write_can

CC = gcc
AR = ar
CXXFLAGS = -O2 -Wall
ARFLAGS = rv

INCDIR = -I../../module

SRCS = read_can.c write_can.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean
all: $(TARGETS)

read_can: read_can.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

write_can: write_can.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

.c.o:
	$(CC) $(CXXFLAGS) -c $< $(INCDIR) $(LIBDIR) $(LIBS)

clean:
	$(RM) $(TARGETS) $(OBJS) *~
