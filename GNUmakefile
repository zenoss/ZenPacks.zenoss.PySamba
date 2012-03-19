###########################################################################
#
# This program is part of Zenoss Core, an open source monitoring platform.
# Copyright (C) 2012, Zenoss Inc.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 or (at your
# option) any later version as published by the Free Software Foundation.
#
# For complete information please visit: http://www.zenoss.com/oss/
#
###########################################################################

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
	if ! make debug DESTDIR=$(ZENPACK_DIR); then \
		exit 1 ;\
	fi ;\
	if ! make build DESTDIR=$(ZENPACK_DIR); then \
		exit 1 ;\
	fi ;\
	if ! make install DESTDIR=$(ZENPACK_DIR); then \
		exit 1 ;\
	fi

clean:
	-rm -rf build dist *.egg-info 2>/dev/null
	find . -name '*.pyc' -delete
	cd $(WMI_DIR) ; \
	make clean DESTDIR=$(ZENPACK_DIR)
