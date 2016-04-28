ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
-include ../../Makefile.include

ifeq ($(NATIVEPREFIX),)
  PREFIX = $(ROOT_DIR)
else
  PREFIX = $(NATIVEPREFIX)
endif

ifeq ($(NATIVEPLATFORM),)
  PLATFORM = native
else
  PLATFORM = $(NATIVEPLATFORM)
endif

SOURCE=$(ROOT_DIR)/src

CONFIGURE=$(SOURCE)/configure --prefix=$(PREFIX)
APP=$(PREFIX)/bin/JsonSchemaBuilder

all: $(APP)

$(SOURCE)/configure: $(SOURCE)/configure.ac
	cd $(SOURCE) && ./autogen.sh

$(PLATFORM)/config.log: $(SOURCE)/configure
	rm -rf $(PLATFORM)
	mkdir -p $(PLATFORM)
	cd $(PLATFORM) && $(CONFIGURE)

$(PLATFORM)/JsonSchemaBuilder: $(PLATFORM)/config.log
	$(MAKE) -C $(PLATFORM)

$(APP): $(PLATFORM)/JsonSchemaBuilder
	$(MAKE) -C $(PLATFORM) install

clean:
	$(MAKE) -C $(PLATFORM) clean

distclean::
	rm -rf $(PLATFORM)
	-rm -rf bin
