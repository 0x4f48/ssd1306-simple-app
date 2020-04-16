TARGET := ipshow
CC := $(CROSS_COMPILE)gcc
CFLAGS := -MMD -I../lib
LDFLAGS := -L../lib
LIBS :=-lssd1306

SRC  := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRC))
DEPS := $(patsubst %.c, %.d, $(SRC))

all: $(OBJS)
	$(CC) -o $(TARGET) $^ $(LDFLAGS) $(LIBS)

%.o: %c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(DEPS)

.PHONY: clean

-include $(DEPS)
