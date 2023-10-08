
PROFILE := cpbf

SKETCH := $(shell basename $(CURDIR))
SOURCES := $(wildcard $(SKETCH)/*)

BUILD_DIR = build
BINFILE = $(BUILD_DIR)/$(SKETCH).ino.bin

ARDUINO_CLI = arduino-cli --profile $(PROFILE)
PORT_PATTERN = /dev/cu.usbmodem[0-9]+
BAUD = 115200

ifeq ($(USE_RTC),1)
	BUILD_PROPERTIES := --build-property compiler.cpp.extra_flags="-DUSE_RTC=1"
else
	BUILD_PROPERTIES :=
endif

.PHONY: all clean compile dump properties upload

all: $(BINFILE) $(FS_IMAGE)

$(BINFILE): $(SOURCES)
	$(ARDUINO_CLI) compile $(BUILD_PROPERTIES) --output-dir $(BUILD_DIR) $(SKETCH)

clean:
	@rm -rf $(BUILD_DIR)

compile: $(BINFILE)

dump: $(CFG_FILE)
	arduino-cli config dump

monitor: port_identified
	arduino-cli monitor -p $(PORT) -c baudrate=$(BAUD)

port_identified:
	$(eval PORT=$(shell arduino-cli board list | egrep "$(PORT_PATTERN)" | cut -d ' ' -f 1))
	@if [ -z "$(PORT)" ]; then \
		echo "Error: No board attached matching $(PORT_PATTERN)"; \
		exit 1; \
	fi

properties:
	$(ARDUINO_CLI) compile --show-properties $(SKETCH)

upload: $(BINFILE) port_identified
	$(ARDUINO_CLI) upload -p $(PORT) $(SKETCH)
