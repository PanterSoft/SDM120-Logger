.PHONY: all build clean run flash

SUBDIR ?= Modbus-Logger

all: build

build:
	cd $(SUBDIR) && python3 idf.py build

clean:
	cd $(SUBDIR) && python3 idf.py clean

run:
	cd $(SUBDIR) && python3 idf.py monitor

flash:
	cd $(SUBDIR) && python3 idf.py flash