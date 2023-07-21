#.rst
# FindLibRGA
# -------------
#
# Finds the LibRGA library.
#
# This will define the following variables:
#
# ``LIBRGA_FOUND``
#     True if the requested version of gcrypt was found
# ``LIBRGA_VERSION``
#     The version of gcrypt that was found
# ``LIBRGA_INCLUDE_DIRS``
#     The gcrypt include directories
# ``LIBRGA_LIBRARIES``
#     The linker libraries needed to use the gcrypt library

# Copyright 2023 FMSoft.CN <https://www.fmsoft.cn>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(LIBRGA_LIB_HINT "${PREFIX}/lib")
set(LIBRGA_INCLUDE_HINT "${PREFIX}/include")

find_library(LIBRGA_LIBRARY
    NAMES rga
    HINTS ${LIBRGA_LIB_HINT}
)

find_path(LIBRGA_INCLUDE_DIR
    NAMES im2d.h
    HINTS ${LIBRGA_INCLUDE_HINT}
)

if (LIBRGA_INCLUDE_DIR AND LIBRGA_LIBRARY)
    if (EXISTS "${LIBRGA_INCLUDE_DIR}/im2d_version.h")
        set(LIBRGA_FOUND "ON")

        file(READ "${LIBRGA_INCLUDE_DIR}/im2d_version.h" LIBRGA_VERSION_CONTENT)

        string(REGEX MATCH "#define +RGA_API_MAJOR_VERSION +([0-9]+)" _dummy "${LIBRGA_VERSION_CONTENT}")
        set(LIBRGA_VERSION_MAJOR "${CMAKE_MATCH_1}")

        string(REGEX MATCH "#define +RGA_API_MINOR_VERSION +([0-9]+)" _dummy "${LIBRGA_VERSION_CONTENT}")
        set(LIBRGA_VERSION_MINOR "${CMAKE_MATCH_1}")

        string(REGEX MATCH "#define +RGA_API_REVISION_VERSION +([0-9]+)" _dummy "${LIBRGA_VERSION_CONTENT}")
        set(LIBRGA_VERSION_PATCH "${CMAKE_MATCH_1}")

        set(LIBRGA_VERSION "${LIBRGA_VERSION_MAJOR}.${LIBRGA_VERSION_MINOR}.${LIBRGA_VERSION_PATCH}")
    endif ()
endif ()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LibRGA
    FOUND_VAR LIBRGA_FOUND
    REQUIRED_VARS LIBRGA_LIBRARY LIBRGA_INCLUDE_DIR
    VERSION_VAR LIBRGA_VERSION
)

if (LIBRGA_FOUND)
    set(LIBRGA_LIBRARIES ${LIBRGA_LIBRARY})
    set(LIBRGA_INCLUDE_DIRS ${LIBRGA_INCLUDE_DIR})
endif ()

mark_as_advanced(LIBRGA_LIBRARY LIBRGA_INCLUDE_DIR)

