CC = gcc
CFLAGS = -g -std=c99 -W -Wall `pkg-config --cflags cairo`
LFLAGS = `pkg-config --libs cairo` -ljansson
EXEC = tp2
TEST_IMPL = $(wildcard test*.c)
AUXI_IMPL = $(filter-out $(TEST_IMPL) $(EXEC).c,$(wildcard *.c))
AUXI_OBJS = $(patsubst %.c,%.o,$(AUXI_IMPL))
TEST_OBJS = $(patsubst %.c,%.o,$(TEST_IMPL))
TEST_EXEC = $(patsubst %.c,%,$(TEST_IMPL))

$(EXEC): $(AUXI_OBJS) $(EXEC).o
	$(CC) $(EXEC).o $(AUXI_OBJS) $(LFLAGS) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean fullclean test

clean:
	rm -f *.o
	rm -rf $(EXEC)
	rm -rf $(TEST_EXEC)
	rm -f *.png
	rm -f *.dot

test: $(TEST_OBJS) $(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJS) $(AUXI_OBJS)
	$(CC) $@.o $(AUXI_OBJS) $(LFLAGS) -lcunit -o $@
