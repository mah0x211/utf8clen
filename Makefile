# UTF-8 character length library test Makefile
CC = gcc
# flags for warnings and errors
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c99 \
         -Wformat=2 \
         -Wcast-align -Wcast-qual -Wconversion -Wdouble-promotion \
         -Wfloat-equal -Wpointer-arith -Wshadow -Wuninitialized \
         -Wunused -Wvla -Wwrite-strings -Wstrict-prototypes \
         -Wmissing-prototypes -Wredundant-decls -Winline \
         -fno-common -fstack-protector-strong

# flags for coverage
COV_FLAGS = --coverage -fprofile-arcs -ftest-coverage

# option: flags for Address Sanitizer
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer

TEST_SRC = test/test_utf8clen.c
TEST_BIN = test_utf8clen

.PHONY: all clean test coverage asan report

all: test

test: $(TEST_BIN)
	@echo "Running UTF-8 character length tests..."
	@./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC)
	$(CC) $(CFLAGS) -o $@ $<

# generate coverage report
coverage: clean
	$(CC) $(CFLAGS) $(COV_FLAGS) -o $(TEST_BIN) $(TEST_SRC)
	@echo "Running tests with coverage instrumentation..."
	@./$(TEST_BIN) || true
	@echo "Generating coverage report..."
	@lcov --capture --directory . --output-file coverage.info
	@lcov --ignore-errors unused --remove coverage.info '/usr/include/*' 'test/*' --output-file coverage.info
	@genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report generated in coverage_report/index.html"

# enable Address Sanitizer
asan: clean
	$(CC) $(CFLAGS) $(ASAN_FLAGS) -o $(TEST_BIN) $(TEST_SRC)
	@echo "Running UTF-8 character length tests with Address Sanitizer..."
	@./$(TEST_BIN)

# open coverage report in browser
report: coverage
	open coverage_report/index.html

clean:
	rm -f $(TEST_BIN)
	rm -f *.gcda *.gcno
	rm -f coverage.info
	rm -rf coverage_report
