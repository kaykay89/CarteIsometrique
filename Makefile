SRC_DIR = src
BIN_DIR = bin
EXEC = tp2
PY_DIR = py
PY_TESTS = $(wildcard $(PY_DIR)/*.py)
TEST_EXEC = $(patsubst %.c,%,$(wildcard $(SRC_DIR)/test*.c))

.PHONY: bindir exec clean source test testpy testbin testcunit

exec: source bindir
	cp $(SRC_DIR)/tp2 $(BIN_DIR)

bindir:
	mkdir -p $(BIN_DIR)

clean:
	$(MAKE) clean -C $(SRC_DIR)
	rm -rf $(BIN_DIR)
	rm -f *.png *.pdf *.dot

source:
	$(MAKE) -C $(SRC_DIR)

test: exec testbin testcunit testpy

testpy:
	for test in $(PY_TESTS); do \
		python $$test; \
	done

testbin: source bindir
	$(MAKE) test -C $(SRC_DIR)
	cp $(TEST_EXEC) $(BIN_DIR)

testcunit: testbin
	for test in `ls $(BIN_DIR)/test*` ; do \
		$$test; \
	done
