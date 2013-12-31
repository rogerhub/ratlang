MAKE=make

default: compile

compile:
	$(MAKE) -C src compile

clean:
	$(MAKE) -C src clean

.PHONY: default compile clean
