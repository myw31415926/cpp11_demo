# Makefile entry

UTIL_DIRS   = util/
ALL_DIRS    = $(shell ls -d */)
ALL_DIRS   := $(filter-out $(UTIL_DIRS), $(ALL_DIRS))

all:
	@for dir in $(ALL_DIRS); do \
		echo $$dir; \
		(cd $$dir; make); \
	done

.PHONY: clean
clean:
	@for dir in $(ALL_DIRS); do \
		echo $$dir; \
		(cd $$dir; make clean); \
	done
