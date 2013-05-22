##############################################################################
# 
# Copyright (C) Zenoss, Inc. 2008-2010, all rights reserved.
# 
# This content is made available according to terms specified in
# License.zenoss under the directory where your Zenoss product is installed.
# 
##############################################################################

build:   pywmi-build 
install: pywmi-installed 
all:     build install
.PHONY:  clean debug 

ifeq ($(USE_DESTDIR),0)
export DESTDIR=
endif

WMI_BUILD_TARGETS  = proto bin/wmic bin/winexe libraries
SAMBA_SRCDIR       = Samba/source
ZENPYTHON_SODIR   := $(DESTDIR)$(ZENHOME)/lib
ZENPYTHON          = LD_LIBRARY_PATH=$(ZENPYTHON_SODIR) $(DESTDIR)$(ZENHOME)/bin/python
ZENPYTHON_VERSION := $(shell $(ZENPYTHON) --version 2>&1 | sed -e 's/[Pp]ython[ \t]*/python/' -e 's/\([0-9]\.[0-9]\)\(\.[0-9]\)*/\1/')
ZENHOME_INC        = $(ZENHOME)/include/$(ZENPYTHON_VERSION)
ZENHOME_LIB        = $(ZENHOME)/lib
ZENHOME_BIN        = $(ZENHOME)/bin
PYSAMBA_prefix_LIB = $(PYSAMBA_prefix)/lib
PYSAMBA_prefix_BIN = $(PYSAMBA_prefix)/bin
GET_VERSION        = "import version as v; print v.VERSION"
WMI_VERSION       := $(shell cd $(PYSAMBA_prefix_LIB)/pysamba ; $(ZENPYTHON) -c $(GET_VERSION))
WMI_CPPFLAGS      := -I$(DESTDIR)$(ZENHOME_INC)

#-------------------------------------------------------------------------#
# A key deliverable of this build process is the libasync_wmi shared      #
# library.  Derive the complete filename for this target from config data #
# and the build platform so we know precisely what should get built and   #
# installed.                                                              #
#                                                                         #
# e.g., libasync_wmi_lib.dylib versus libasync_wmi_lib.so.0.0.2           #
#-------------------------------------------------------------------------#
LIBASYNC_WMI_LIB_BASENAME    = libasync_wmi_lib
WMI_CONFIG_MK                = $(SAMBA_SRCDIR)/wmi/config.mk
LIBASYNC_WMI_LIB_VERSION_nnn = $(shell fgrep -A1 "[LIBRARY::async_wmi_lib]" $(WMI_CONFIG_MK) | sed -e "s/^VERSION=\(.*\)/\1/g"    | tail -1)
LIBASYNC_WMI_LIB_VERSION_n   = $(shell fgrep -A2 "[LIBRARY::async_wmi_lib]" $(WMI_CONFIG_MK) | sed -e "s/^SO_VERSION=\(.*\)/\1/g" | tail -1)
ifeq ($(shell uname), Darwin)
LIBASYNC_WMI_LIB             = $(LIBASYNC_WMI_LIB_BASENAME).dylib.$(LIBASYNC_WMI_LIB_VERSION_nnn)
else
# e.g., libasync_wmi_lib.so.0.0.2 and libasync_wmi_lib.so.0 respectively
LIBASYNC_WMI_LIB            := $(LIBASYNC_WMI_LIB_BASENAME).so.$(LIBASYNC_WMI_LIB_VERSION_nnn)
LIBASYNC_WMI_LIB_SO_N       := $(LIBASYNC_WMI_LIB_BASENAME).so.$(LIBASYNC_WMI_LIB_VERSION_n)
endif
PATHED_LIBASYNC_WMI_LIB     := $(SAMBA_SRCDIR)/bin/shared/$(LIBASYNC_WMI_LIB)

#-------------------------------------------------------------------------#
# Google Breakpad Integration                                             #
#-------------------------------------------------------------------------#
# libasync_wmi_lib.so can be built with google-breakpad crash reporting.  #
# http://code.google.com/p/google-breakpad                                #
#                                                                         #
#    Minidumps are typically written to /tmp.                             #
#    See: Samba/source/librpc/rpc/dcerpc.c                                #
#-------------------------------------------------------------------------#
# Comment out the next line to disable google-breakpad dependency.
ifneq ($(shell uname), Darwin)
USE_BREAKPAD = 1
endif

ifneq ($(USE_BREAKPAD),)
breakpad_CPPFLAGS    = -DBREAKPAD
WMI_CPPFLAGS        += $(breakpad_CPPFLAGS)
breakpad_LIB         = libbreakpad_client.a
breakpad_LIBDIR     ?= $(DESTDIR)$(ZENHOME)/lib
_fqp_breakpad_LIB   := $(breakpad_LIBDIR)/$(breakpad_LIB)
fqp_breakpad_LIB     = $(patsubst //%,/%,$(_fqp_breakpad_LIB))
breakpad_LIB_SYMLINK = $(SAMBA_SRCDIR)/bin/static/$(breakpad_LIB)
endif
#-------------------------------------------------------------------------#

# Check existence of a directory or file.  Bail out of the build if it is missing.
#
define check
	@if [ "$1" = "directory" ]; then \
		if [ ! -d "$2" ];then \
			echo $3 | awk '{printf("Missing: %-20s\n",$$1)}';\
			exit 1 ;\
		else \
			echo "$3 $2" | awk '{printf("Found:   %-20s %-20s\n",$$1,$$2)}';\
		fi ;\
	fi
	@if [ "$1" = "file" ]; then \
		if [ ! -f "$2" ];then \
			spFile=`which $2 2>/dev/null` ;\
			if [ ! -f "$${spFile}" ]; then \
				echo "$3 $2" | awk '{printf("Missing: %-20s %s\n",$$1,$$2)}';\
				exit 1 ;\
			else \
				echo "$3 $2 $${spFile}" | awk '{printf("Found:   %-20s %-20s %-20s [in search path]\n",$$1,$$2,$$3)}';\
			fi ;\
		else \
			echo "$3 $2" | awk '{printf("Found:   %-20s %-20s\n",$$1,$$2)}';\
		fi ;\
	fi
endef

build-prereqs:
	@echo
	@echo "Checking prequisites for building WMI"
	$(call check,directory,$(DESTDIR)$(ZENHOME),"ZENHOME")
	$(call check,file,$(ZENPYTHON),"ZENPYTHON")
	$(call check,directory,$(DESTDIR)$(ZENHOME_INC),"ZENHOME_INC")
ifneq ($(USE_BREAKPAD),)
	@if [ ! -f "$(fqp_breakpad_LIB)" ];then \
		echo "Unable to find the google breakpad client library we require at:" ;\
		echo "   $(fqp_breakpad_LIB)" ;\
		echo ;\
		echo "Either comment out USE_BREAKPAD in this makefile or build the" ;\
		echo "breakpad library." ;\
		echo ;\
		exit 1 ;\
	fi
endif
	@touch $@

install-prereqs:
	@echo
	@echo "Checking prequisites for installing WMI"
	$(call check,directory,$(DESTDIR)$(ZENHOME),"ZENHOME")
	$(call check,directory,$(PYSAMBA_prefix_BIN),"PYSAMBA_prefix_BIN")
	$(call check,directory,$(PYSAMBA_prefix_LIB),"PYSAMBA_prefix_LIB")
	@touch $@

LIBRPC_CONFIG_MK = $(SAMBA_SRCDIR)/librpc/config.mk
ifeq ($(USE_BREAKPAD),)
LIBRPC_CONFIG_MK_NOBP = $(SAMBA_SRCDIR)/librpc/config.mk.nobreakpad
$(LIBRPC_CONFIG_MK): $(LIBRPC_CONFIG_MK_NOBP)
	cp $< $@
else
# Tell the build how to link against the breakpad library.
# e.g., Muck with Samba/source/librpc/config.mk to provide that visibility.
#
LIBRPC_CONFIG_MK_BP = $(SAMBA_SRCDIR)/librpc/config.mk.breakpad
LIBRPC_CONFIG_TAG  := $(fqp_breakpad_LIB)
$(LIBRPC_CONFIG_MK): $(LIBRPC_CONFIG_MK_BP)
	sed -e "s|_sed_tag_libbreakpad_client_path_|$(LIBRPC_CONFIG_TAG)|" $< >$@ || rm $@

# Create symlink to actual google breakpad library.
# e.g., Samba/source/bin/static/libbreakpad_client.a -> /actual/path/to/libbreakpad_client.a
$(breakpad_LIB_SYMLINK): $(fqp_breakpad_LIB)
	@if [ ! -d "$(@D)" ];then \
		 mkdir -p $(@D) ;\
	fi
	ln -sf $(fqp_breakpad_LIB) $@
endif

$(SAMBA_SRCDIR)/Makefile: $(SAMBA_SRCDIR)/autogen.sh
	cd $(SAMBA_SRCDIR) ;\
	./autogen.sh ;\
	CPPFLAGS="$(WMI_CPPFLAGS)" ./configure --without-readline --enable-debug

ifeq ($(USE_BREAKPAD),)
pywmi-build: build-prereqs $(LIBRPC_CONFIG_MK) $(SAMBA_SRCDIR)/Makefile
else
pywmi-build: build-prereqs $(LIBRPC_CONFIG_MK) $(SAMBA_SRCDIR)/Makefile $(breakpad_LIB_SYMLINK)
endif
	cd $(SAMBA_SRCDIR);\
	$(MAKE) $(WMI_BUILD_TARGETS) ;\
	touch $@

pywmi-installed: $(PYSAMBA_prefix_LIB) $(PYSAMBA_prefix_BIN) install-prereqs $(SAMBA_SRCDIR)/bin/wmic $(SAMBA_SRCDIR)/bin/winexe $(PATHED_LIBASYNC_WMI_LIB)
	cp $(SAMBA_SRCDIR)/bin/wmic   $(PYSAMBA_prefix_BIN)
	cp $(SAMBA_SRCDIR)/bin/winexe $(PYSAMBA_prefix_BIN)
ifeq ($(shell uname), Darwin)
	cp $(PATHED_LIBASYNC_WMI_LIB) $(PYSAMBA_prefix_LIB)/$(LIBASYNC_WMI_LIB_BASENAME).$(LIBASYNC_WMI_LIB_VERSION_nnn).dylib
	(cd $(PYSAMBA_prefix_LIB) && ln -sf $(LIBASYNC_WMI_LIB_BASENAME).$(LIBASYNC_WMI_LIB_VERSION_nnn).dylib $(LIBASYNC_WMI_LIB_BASENAME).dylib)
else
	-(cd $(PYSAMBA_prefix_LIB) && rm -f $(LIBASYNC_WMI_LIB_BASENAME)*)
	cp $(PATHED_LIBASYNC_WMI_LIB) $(PYSAMBA_prefix_LIB)
	(cd $(PYSAMBA_prefix_LIB) && ln -sf $(LIBASYNC_WMI_LIB) $(LIBASYNC_WMI_LIB_SO_N))
endif

$(PYSAMBA_prefix_LIB) $(PYSAMBA_prefix_BIN):
	mkdir -p $@

clean: $(LIBRPC_CONFIG_MK)
	-if [ -f "$(SAMBA_SRCDIR)/Makefile" ] ; then\
		cd $(SAMBA_SRCDIR) ;\
		make distclean ;\
	fi
	rm -f $(SAMBA_SRCDIR)/bin/shared/* 
	rm -f $(SAMBA_SRCDIR)/bin/static/* 
	rm -f $(SAMBA_SRCDIR)/bin/wmic
	rm -f $(SAMBA_SRCDIR)/bin/winexe 
	rm -f $(SAMBA_SRCDIR)/heimdal/lib/des/hcrypto
	rm -f $(PYSAMBA_prefix_BIN)/wmic
	rm -f $(PYSAMBA_prefix_BIN)/winexe
	rm -f build-prereqs
	rm -f install-prereqs
	rm -f $(LIBRPC_CONFIG_MK)
	rm -f $(PYSAMBA_prefix_LIB)/$(LIBASYNC_WMI_LIB_BASENAME)*
	@-[ -L $(breakpad_LIB_SYMLINK) ] && rm -f $(breakpad_LIB_SYMLINK)

debug:	
	@echo "WMI_VERSION       = $(WMI_VERSION)"
	@echo "SAMBA_SRCDIR      = $(SAMBA_SRCDIR)"
	@echo "PYSAMBA_prefix    = $(PYSAMBA_prefix)"
	@echo "PYSAMBA_prefix_BIN= $(PYSAMBA_prefix_BIN)"
	@echo "PYSAMBA_prefix_LIB= $(PYSAMBA_prefix_LIB)"
	@echo "DESTDIR           = $(DESTDIR)"
	@echo "ZENHOME_BIN       = $(ZENHOME_BIN)"
	@echo "ZENHOME_LIB       = $(ZENHOME_LIB)"
	@echo "ZENHOME_INC       = $(ZENHOME_INC)"
	@echo "ZENPYTHON         = $(ZENPYTHON)"
	@echo "ZENPYTHON_VERSION = $(ZENPYTHON_VERSION)"
	@echo "WMI_CONFIGURE       CPPFLAGS="$(WMI_CPPFLAGS)" ./configure --without-readline --enable-debug"
	@echo "WMI_MAKE            $(MAKE) $(WMI_BUILD_TARGETS)"
ifeq ($(USE_BREAKPAD),)
	@echo "USE_BREAKPAD        [ disabled ]"
else
	@echo "LIBRPC_CONFIG_TAG = $(LIBRPC_CONFIG_TAG)"
	@echo "USE_BREAKPAD        [ enabled ]"
	@echo "breakpad_CPPFLAGS = $(breakpad_CPPFLAGS)"
	@echo "breakpad_LIB      = $(breakpad_LIB)"
	@echo "breakpad_LIBDIR   = $(breakpad_LIBDIR)"
	@echo "fqp_breakpad_LIB  = $(fqp_breakpad_LIB)"
endif
	@echo "ZENHOME           = $(ZENHOME)"
	@if [ -z "$(ZENHOME)" ] ;then \
		echo ;\
		echo "Error: ZENHOME is not set." ;\
		echo "       This can cause problems locating a good version of python for this build." ;\
		echo ;\
		exit 1 ;\
	fi
	@if [ -z "$(ZENPYTHON_VERSION)" ] ;then \
		echo ;\
		echo "Error: ZENPYTHON_VERSION is not set." ;\
		echo "       Please investigate." ;\
		echo ;\
		exit 1 ;\
	fi
	@if [ -z "$(WMI_VERSION)" ] ;then \
		echo ;\
		echo "Error: WMI_VERSION is not set." ;\
		echo "       Please investigate." ;\
		echo ;\
		exit 1 ;\
	fi
