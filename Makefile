TARGET := ssd1306
CC := gcc
CFLAGS := -MMD

SRC  := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRC))
DEPS := $(patsubst %.c, %.d, $(SRC))

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) 

%.o: %c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(DEPS)

.PHONY: clean

-include $(DEPS)
