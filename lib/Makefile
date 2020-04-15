TARGET := libssd1306.a
CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
CFLAGS := -MMD

SRC  := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRC))
DEPS := $(patsubst %.c, %.d, $(SRC))

all: $(OBJS)
	$(AR) rcs $(TARGET) $^

%.o: %c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(DEPS)

.PHONY: clean

-include $(DEPS)
