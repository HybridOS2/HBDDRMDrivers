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
FindLibUDEV
--------------

Find LibUDEV headers and libraries.

Imported Targets
^^^^^^^^^^^^^^^^

``LibUDEV::LibUDEV``
  The LibUDEV library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables in your project:

``LibUDEV_FOUND``
  true if (the requested version of) LibUDEV is available.
``LibUDEV_VERSION``
  the version of LibUDEV.
``LibUDEV_LIBRARIES``
  the libraries to link against to use LibUDEV.
``LibUDEV_INCLUDE_DIRS``
  where to find the LibUDEV headers.
``LibUDEV_COMPILE_OPTIONS``
  this should be passed to target_compile_options(), if the
  target is not used for linking

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LIBUDEV QUIET libudev)
set(LibUDEV_COMPILE_OPTIONS ${PC_LIBUDEV_CFLAGS_OTHER})
set(LibUDEV_VERSION ${PC_LIBUDEV_VERSION})

find_path(LibUDEV_INCLUDE_DIR
    NAMES libudev.h
    HINTS ${PC_LIBUDEV_INCLUDEDIR} ${PC_LIBUDEV_INCLUDE_DIR}
)

find_library(LibUDEV_LIBRARY
    NAMES ${LibUDEV_NAMES} udev
    HINTS ${PC_LIBUDEV_LIBDIR} ${PC_LIBUDEV_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibUDEV
    FOUND_VAR LibUDEV_FOUND
    REQUIRED_VARS LibUDEV_LIBRARY LibUDEV_INCLUDE_DIR
    VERSION_VAR LibUDEV_VERSION
)

if (LibUDEV_LIBRARY AND NOT TARGET LibUDEV::LibUDEV)
    add_library(LibUDEV::LibUDEV UNKNOWN IMPORTED GLOBAL)
    set_target_properties(LibUDEV::LibUDEV PROPERTIES
        IMPORTED_LOCATION "${LibUDEV_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${LibUDEV_COMPILE_OPTIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibUDEV_INCLUDE_DIR}"
    )
endif ()

mark_as_advanced(LibUDEV_INCLUDE_DIR LIBUDEV_LIBRARIES)

if (LibUDEV_FOUND)
    set(LibUDEV_LIBRARIES ${LibUDEV_LIBRARY})
    set(LibUDEV_INCLUDE_DIRS ${LibUDEV_INCLUDE_DIR})
endif ()

