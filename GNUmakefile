##############################################################################
# 
# Copyright (C) Zenoss, Inc. 2012, all rights reserved.
# 
# This content is made available according to terms specified in
# License.zenoss under the directory where your Zenoss product is installed.
# 
##############################################################################


# This will build the components of the C Samba library used by other ZenPacks.

PYTHON=python
WMI_SRCDIR=$(PWD)/src/wmi
PYSAMBA_SRCDIR=$(PWD)/ZenPacks/zenoss/PySamba

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
	cd $(WMI_SRCDIR) ; \
	if ! make debug PYSAMBA_SRCDIR=$(PYSAMBA_SRCDIR); then \
		exit 1 ;\
	fi ;\
	if ! make build PYSAMBA_SRCDIR=$(PYSAMBA_SRCDIR); then \
		exit 1 ;\
	fi ;\
	if ! make install PYSAMBA_SRCDIR=$(PYSAMBA_SRCDIR); then \
		exit 1 ;\
	fi

clean:
	-rm -rf build dist *.egg-info 2>/dev/null
	find . -name '*.pyc' -delete
	cd $(WMI_SRCDIR) ; \
	make clean PYSAMBA_SRCDIR=$(PYSAMBA_SRCDIR)
