include(GNUInstallDirs)

DRMDRIVERS_OPTION_BEGIN()

CALCULATE_LIBRARY_VERSIONS_FROM_LIBTOOL_TRIPLE(DRMDRIVERS 0 0 0)

# These are shared variables, but we special case their definition so that we can use the
# CMAKE_INSTALL_* variables that are populated by the GNUInstallDirs macro.
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBDIR}" CACHE PATH "Absolute path to library installation directory")
set(EXEC_INSTALL_DIR "${CMAKE_INSTALL_FULL_BINDIR}" CACHE PATH "Absolute path to executable installation directory")
set(LIBEXEC_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}/drmdrivers" CACHE PATH "Absolute path to install executables executed by the library")
set(HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Absolute path to header installation directory")
set(DRMDRIVERS_HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/drmdrivers" CACHE PATH "Absolute path to DRMDrivers header installation directory")

add_definitions(-DBUILDING_LINUX__=1)
add_definitions(-DDRMDRIVERS_API_VERSION_STRING="${DRMDRIVERS_API_VERSION}")

find_package(LibDRM 2.4.0 REQUIRED)
find_package(LibUDEV 200 REQUIRED)
find_package(MiniGUI 5.0.0 REQUIRED)

if (LibDRM_FOUND)

    DRMDRIVERS_CHECK_HAVE_INCLUDE(HAVE_VMWGFX_DRM_H "${LibDRM_INCLUDE_DIR}/vmwgfx_drm.h")

    find_package(LibDRMIntel 2.4.0)
    if (NOT LibDRMIntel_FOUND)
        SET_AND_EXPOSE_TO_BUILD(HAVE_DRM_INTEL OFF)
    else ()
        SET_AND_EXPOSE_TO_BUILD(HAVE_DRM_INTEL ON)
    endif ()
endif ()

# Public options specific to the HybridOS port. Do not add any options here unless
# there is a strong reason we should support changing the value of the option,
# and the option is not relevant to any other DRMDrivers ports.
#DRMDRIVERS_OPTION_DEFINE(USE_SYSTEMD "Whether to enable journald logging" PUBLIC ON)

# Finalize the value for all options. Do not attempt to use an option before
# this point, and do not attempt to change any option after this point.
DRMDRIVERS_OPTION_END()

# CMake does not automatically add --whole-archive when building shared objects from
# a list of convenience libraries. This can lead to missing symbols in the final output.
# We add --whole-archive to all libraries manually to prevent the linker from trimming
# symbols that we actually need later. With ld64 on darwin, we use -all_load instead.
macro(ADD_WHOLE_ARCHIVE_TO_LIBRARIES _list_name)
    if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        list(APPEND ${_list_name} -Wl,-all_load)
    else ()
        set(_tmp)
        foreach (item IN LISTS ${_list_name})
            if ("${item}" STREQUAL "PRIVATE" OR "${item}" STREQUAL "PUBLIC")
                list(APPEND _tmp "${item}")
            else ()
                list(APPEND _tmp -Wl,--whole-archive "${item}" -Wl,--no-whole-archive)
            endif ()
        endforeach ()
        set(${_list_name} ${_tmp})
    endif ()
endmacro()

set(DRMDrivers_PKGCONFIG_FILE ${CMAKE_BINARY_DIR}/source/drmdrivers/hbddrmdrivers.pc)

set(DRMDrivers_LIBRARY_TYPE SHARED)
#include(BubblewrapSandboxChecks)
