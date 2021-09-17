
TARGET = apt_usbtrx_resetts

DESTDIR ?= /usr/local

CC = gcc
AR = ar
CXXFLAGS = -O2 -Wall
ARFLAGS = rv
LDFLAGS=-static-libstdc++

INCDIR = -I../../module

SRCS = apt_usbtrx_resetts.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

.c.o:
	$(CC) $(CXXFLAGS) -c $< $(INCDIR) $(LIBDIR) $(LIBS)

.PHONY: clean install
clean:
	$(RM) $(TARGET) $(OBJS) *~

install:
	install -d $(DESTDIR)/bin
	install $(TARGET) $(DESTDIR)/bin
