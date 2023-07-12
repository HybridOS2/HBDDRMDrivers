# Copyright (C) 2023 FMSoft.CN
# Copyright (C) 2018, 2019 Sony Interactive Entertainment Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND ITS CONTRIBUTORS ``AS
# IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#[=======================================================================[.rst:
FindLibDRMIntel
--------------

Find LibDRMIntel headers and libraries.

Imported Targets
^^^^^^^^^^^^^^^^

``LibDRMIntel::LibDRMIntel``
  The LibDRMIntel library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables in your project:

``LibDRMIntel_FOUND``
  true if (the requested version of) LibDRMIntel is available.
``LibDRMIntel_VERSION``
  the version of LibDRMIntel.
``LibDRMIntel_LIBRARIES``
  the libraries to link against to use LibDRMIntel.
``LibDRMIntel_INCLUDE_DIRS``
  where to find the LibDRMIntel headers.
``LibDRMIntel_COMPILE_OPTIONS``
  this should be passed to target_compile_options(), if the
  target is not used for linking

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LIBDRMINTEL QUIET libdrm_intel)
set(LibDRMIntel_COMPILE_OPTIONS ${PC_LIBDRMINTEL_CFLAGS_OTHER})
set(LibDRMIntel_VERSION ${PC_LIBDRMINTEL_VERSION})

find_path(LibDRMIntel_INCLUDE_DIR
    NAMES intel_bufmgr.h
    HINTS ${PC_LIBDRMINTEL_INCLUDEDIR} ${PC_LIBDRMINTEL_INCLUDE_DIR}
    PATH_SUFFIXES libdrm
)

find_library(LibDRMIntel_LIBRARY
    NAMES ${LibDRMIntel_NAMES} drm_intel
    HINTS ${PC_LIBDRMINTEL_LIBDIR} ${PC_LIBDRMINTEL_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibDRMIntel
    FOUND_VAR LibDRMIntel_FOUND
    REQUIRED_VARS LibDRMIntel_LIBRARY LibDRMIntel_INCLUDE_DIR
    VERSION_VAR LibDRMIntel_VERSION
)

if (LibDRMIntel_LIBRARY AND NOT TARGET LibDRMIntel::LibDRMIntel)
    add_library(LibDRM::LibDRMIntel UNKNOWN IMPORTED GLOBAL)
    set_target_properties(LibDRM::LibDRMIntel PROPERTIES
        IMPORTED_LOCATION "${LibDRMIntel_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${LibDRMIntel_COMPILE_OPTIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibDRMIntel_INCLUDE_DIR}"
    )
endif ()

mark_as_advanced(LibDRMIntel_INCLUDE_DIR LIBDRMINTEL_LIBRARIES)

if (LibDRMIntel_FOUND)
    set(LibDRMIntel_LIBRARIES ${LibDRMIntel_LIBRARY})
    set(LibDRMIntel_INCLUDE_DIRS ${LibDRMIntel_INCLUDE_DIR})
endif ()

