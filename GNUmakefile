# GNUmakefile for the pysamba library

# This will build the components of the C Samba library used by other ZenPacks.

PYTHON=python
SRC_DIR=$(PWD)/src
WMI_DIR=$(SRC_DIR)/wmi
ZENPACK_DIR=$(PWD)/ZenPacks/zenoss/PySamba

# Default target. This won't be used by any automated process, but would be
# used if you simply ran "make" in this directory.
default: build

# The build target is specifically executed each time setup.py executes.
# Typically this is when the ZenPack is being built into an egg, or when it is
# installed using the zenpack --link option to install in development mode.
build:
	# Build WMI - 
	# Note that by default, we require the google breakpad client 
	# library be available on this machine in $ZENHOME/lib/. This
	# is provided in core.
	echo "Building WMI libraries..."; \
	cd $(WMI_DIR) ; \
	configure; \
	make debug DESTDIR=$(ZENPACK_DIR); \
	make build DESTDIR=$(ZENPACK_DIR); \
	make install DESTDIR=$(ZENPACK_DIR); \

# The clean target won't be used by any automated process.
clean:
	rm -rf build dist *.egg-info
	find . -name '*.pyc' | xargs rm

	cd $(WMI_DIR) ; \
	make clean DESTDIR=$(ZENPACK_DIR)

