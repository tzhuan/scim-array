## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2007 Yu-Chun Wang
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


array_DEFINES		= -DSCIM_ICONDIR=\"@SCIM_ICONDIR@\" \
					  -DSCIM_ARRAY_LOCALEDIR=\"$(datadir)/locale\" \
					  -DSCIM_ARRAY_TABLEDIR=\"@SCIM_SHAREDIR@/Array\" \
					  -DSCIM_ARRAY_ICON_FILE=\"@SCIM_ICONDIR@/scim-array.png\"

noinst_HEADERS		= scim_array_imengine.h ArrayCIN.h

moduledir		= @SCIM_MODULEDIR@/IMEngine
module_LTLIBRARIES	= array.la

array_la_SOURCES 	= scim_array_imengine.cpp ArrayCIN.cpp

array_la_CFLAGS	= @SCIM_CFLAGS@ $(array_DEFINES)
array_la_CXXFLAGS	= @SCIM_CFLAGS@ $(array_DEFINES)

array_la_LDFLAGS	= -avoid-version \
		     	  -rpath $(moduledir) \
			  -module \
			  @INTLLIBS@ \
			  @SCIM_LIBS@


if SCIM_BUILD_SETUP
SETUP_MODULE = array-imengine-setup.la
endif

setupmoduledir           = @SCIM_MODULEDIR@/SetupUI
setupmodule_LTLIBRARIES  = $(SETUP_MODULE)

array_imengine_setup_la_SOURCES = scim_array_imengine_setup.cpp

array_imengine_setup_la_CFLAGS  = @SCIM_GTKUTILS_CFLAGS@ \
								   @SCIM_CFLAGS@ \
								   $(array_DEFINES)

array_imengine_setup_la_CXXFLAGS= @SCIM_GTKUTILS_CFLAGS@ \
								   @SCIM_CFLAGS@ \
								   $(array_DEFINES)

array_imengine_setup_la_LDFLAGS = -avoid-version \
								   -rpath $(setupmoduledir) \
								   -module \
								   @LIBTOOL_EXPORT_OPTIONS@ \
								   @SCIM_GTKUTILS_LIBS@ \
								   @INTLLIBS@ \
								   @SCIM_LIBS@
