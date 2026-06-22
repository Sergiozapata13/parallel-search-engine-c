CC = gcc

CPPFLAGS = -Iinclude
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -O2 -pthread -D_POSIX_C_SOURCE=200809L
DEBUG_FLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -O0 -g3 -pthread -D_POSIX_C_SOURCE=200809L
SANITIZE_FLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -O1 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer -pthread -D_POSIX_C_SOURCE=200809L

SRC = src/main.c \
      src/config.c \
      src/charset.c \
      src/search.c \
      src/scheduler.c \
      src/timer.c \
      src/metrics.c

BIN = parallel_search

.PHONY: all debug sanitize clean run test valgrind benchmark check

all:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC) -o $(BIN)

debug:
	$(CC) $(CPPFLAGS) $(DEBUG_FLAGS) $(SRC) -o $(BIN)

sanitize:
	$(CC) $(CPPFLAGS) $(SANITIZE_FLAGS) $(SRC) -o $(BIN)

run: all
	./$(BIN) config/example.conf

test: all
	./$(BIN) config/example.conf --threads 1
	./$(BIN) config/example.conf --threads 2
	./$(BIN) config/example.conf --threads 4
	./$(BIN) config/benchmark.conf --threads 4 --full-scan

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN) config/example.conf --threads 4 --full-scan

benchmark:
	RUNS=1 THREADS="1 2 4" scripts/run_benchmarks.sh

check: clean all test sanitize
	./$(BIN) config/example.conf --threads 4 --full-scan

clean:
	rm -f $(BIN)
