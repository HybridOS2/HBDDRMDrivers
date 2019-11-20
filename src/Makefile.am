# Copyright © 2019 FMSoft Technologies
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
# Authors:
#    WEI Yongming <vincent@minigui.org>

AUTOMAKE_OPTIONS = subdir-objects

include Makefile.sources

AM_CFLAGS = -I$(DRM_INC_DIR)

libhidrmdrivers_la_LTLIBRARIES = libhidrmdrivers.la
libhidrmdrivers_la_LDFLAGS = -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
libhidrmdrivers_la_LIBADD = $(DEP_LIBS)

LIBHIDRMDRIVERS_SOURCE_FILES = \
    drivers.c \
    drivers.h \
    $(NULL)

LIBHIDRMDRIVERS_HEADER_FILES = \
    $(NULL)

if DRM_INTEL
LIBHIDRMDRIVERS_SOURCE_FILES += $(DRM_INTEL_SOURCE_FILES)
LIBHIDRMDRIVERS_HEADER_FILES += $(DRM_INTEL_HEADER_FILES)
endif

libhidrmdrivers_ladir = $(libdir)
libhidrmdrivers_la_SOURCES = $(LIBHIDRMDRIVERS_SOURCE_FILES)

libhidrmdriversincludedir = ${includedir}
libhidrmdriversinclude_HEADERS = $(LIBHIDRMDRIVERS_HEADER_FILES)

