.RECIPEPREFIX = >

CC      = gcc
CFLAGS  = -Wall -Wextra -std=gnu11 -g -Iinclude
LDLIBS  = -lreadline
TARGET  = shellforge
SRCS    = $(wildcard src/*.c)
OBJS    = $(SRCS:src/%.c=build/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
> $(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.c | build
> $(CC) $(CFLAGS) -MMD -MP -c $< -o $@

build:
> mkdir -p build

clean:
> rm -rf build $(TARGET)

-include $(OBJS:.o=.d)

.PHONY: all clean
