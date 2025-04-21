
TARGET = ep1_cf02a_set_host_resetts

DESTDIR ?= /usr/local

CC = gcc
AR = ar
CXXFLAGS = -O2 -Wall
ARFLAGS = rv
LDFLAGS=-static-libstdc++

INCDIR = -I../../../module

SRCS = ep1_cf02a_set_host_resetts.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

.c.o:
	$(CC) $(CXXFLAGS) -c $< $(INCDIR) $(LIBDIR) $(LIBS)

.PHONY: clean install uninstall
clean:
	$(RM) $(TARGET) $(OBJS) *~

install:
	install -d $(DESTDIR)/bin
	install $(TARGET) $(DESTDIR)/bin

uninstall:
	$(RM) $(DESTDIR)/bin/$(TARGET)
