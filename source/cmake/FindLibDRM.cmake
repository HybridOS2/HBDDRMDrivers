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
FindLibDRM
--------------

Find LibDRM headers and libraries.

Imported Targets
^^^^^^^^^^^^^^^^

``LibDRM::LibDRM``
  The LibDRM library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables in your project:

``LibDRM_FOUND``
  true if (the requested version of) LibDRM is available.
``LibDRM_VERSION``
  the version of LibDRM.
``LibDRM_LIBRARIES``
  the libraries to link against to use LibDRM.
``LibDRM_INCLUDE_DIRS``
  where to find the LibDRM headers.
``LibDRM_COMPILE_OPTIONS``
  this should be passed to target_compile_options(), if the
  target is not used for linking

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LIBDRM QUIET libdrm)
set(LibDRM_COMPILE_OPTIONS ${PC_LIBDRM_CFLAGS_OTHER})
set(LibDRM_VERSION ${PC_LIBDRM_VERSION})

find_path(LibDRM_INCLUDE_DIR
    NAMES drm.h
    HINTS ${PC_LIBDRM_INCLUDEDIR} ${PC_LIBDRM_INCLUDE_DIR}
    PATH_SUFFIXES libdrm
)

find_library(LibDRM_LIBRARY
    NAMES ${LibDRM_NAMES} drm
    HINTS ${PC_LIBDRM_LIBDIR} ${PC_LIBDRM_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibDRM
    FOUND_VAR LibDRM_FOUND
    REQUIRED_VARS LibDRM_LIBRARY LibDRM_INCLUDE_DIR
    VERSION_VAR LibDRM_VERSION
)

if (LibDRM_LIBRARY AND NOT TARGET LibDRM::LibDRM)
    add_library(LibDRM::LibDRM UNKNOWN IMPORTED GLOBAL)
    set_target_properties(LibDRM::LibDRM PROPERTIES
        IMPORTED_LOCATION "${LibDRM_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${LibDRM_COMPILE_OPTIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibDRM_INCLUDE_DIR}"
    )
endif ()

mark_as_advanced(LibDRM_INCLUDE_DIR LIBDRM_LIBRARIES)

if (LibDRM_FOUND)
    set(LibDRM_LIBRARIES ${LibDRM_LIBRARY})
    set(LibDRM_INCLUDE_DIRS ${LibDRM_INCLUDE_DIR})
endif ()

