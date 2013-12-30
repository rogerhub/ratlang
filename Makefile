MAKE=make

default: compile

compile:
	$(MAKE) -C src compile

.PHONY: default compile
