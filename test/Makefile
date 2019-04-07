# Makefile entry

TARGET_DIRS = _build/
TEST_DIRS    = $(shell ls -d */)
TEST_DIRS   := $(filter-out $(TARGET_DIRS), $(TEST_DIRS))

all:
	mkdir -p $(TARGET_DIRS)
	@for dir in $(TEST_DIRS); do \
		echo $$dir; \
		(cd $$dir; make); \
	done

.PHONY: clean
clean:
	@for dir in $(TEST_DIRS); do \
		echo $$dir; \
		(cd $$dir; make clean); \
	done
