set_property(DIRECTORY . PROPERTY FOLDER "DRMDrivers")
set(DRMDrivers_OUTPUT_NAME "drmdrivers")

include(GlobalCommon)

set(DRMDrivers_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
    "${DRMDRIVERS_DIR}"
    "${DRMDRIVERS_DIR}/include"
    "${DRMDrivers_DERIVED_SOURCES_DIR}"
    "${FORWARDING_HEADERS_DIR}"
    "${LibDRM_INCLUDE_DIR}"
)

list(APPEND DRMDrivers_SOURCES
    "${DRMDRIVERS_DIR}/common/drivers.c"
    "${DRMDRIVERS_DIR}/common/helpers.c"
)

if (HAVE_DRM_INTEL)
    list(APPEND DRMDrivers_SOURCES
        "${DRMDRIVERS_DIR}/intel/intel-chipset.c"
        "${DRMDRIVERS_DIR}/intel/intel-i915-driver.c"
    )
endif ()

if (HAVE_VMWGFX_DRM_H)
    list(APPEND DRMDrivers_SOURCES
        "${DRMDRIVERS_DIR}/vmwgfx/vmwgfx.c"
    )
endif ()

set(DRMDrivers_LIBRARIES
    ${LibDRM_LIBRARIES}
    ${LibUDEV_LIBRARIES}
)

if (HAVE_DRM_INTEL)
    list(APPEND DRMDrivers_LIBRARIES
        ${LibDRMIntel_LIBRARIES}
    )
endif ()

if (HAVE_LIBRGA)
    list(APPEND DRMDrivers_SOURCES
        "${DRMDRIVERS_DIR}/rockchip/rockchip-drm.c"
    )

    list(APPEND DRMDrivers_LIBRARIES
        ${LIBRGA_LIBRARIES}
    )
endif ()

set(DRMDrivers_INTERFACE_LIBRARIES DRMDrivers)
set(DRMDrivers_INTERFACE_INCLUDE_DIRECTORIES ${DRMDrivers_PRIVATE_FRAMEWORK_HEADERS_DIR})

DRMDRIVERS_FRAMEWORK_DECLARE(DRMDrivers)
DRMDRIVERS_INCLUDE_CONFIG_FILES_IF_EXISTS()

DRMDRIVERS_WRAP_SOURCELIST(${DRMDrivers_SOURCES})

DRMDRIVERS_COPY_FILES(DRMDrivers_CopyPrivateHeaders
    DESTINATION ${DRMDrivers_PRIVATE_FRAMEWORK_HEADERS_DIR}/drmdrivers
    FILES ${DRMDrivers_PRIVATE_FRAMEWORK_HEADERS}
    FLATTENED
)
list(APPEND DRMDrivers_INTERFACE_DEPENDENCIES DRMDrivers_CopyPrivateHeaders)
if (NOT INTERNAL_BUILD)
    add_dependencies(DRMDrivers_CopyPrivateHeaders DRMDrivers)
endif ()

DRMDRIVERS_COMPUTE_SOURCES(DRMDrivers)
DRMDRIVERS_FRAMEWORK(DRMDrivers)
DRMDRIVERS_FRAMEWORK_TARGET(DRMDrivers)

if (${DRMDrivers_LIBRARY_TYPE} MATCHES "SHARED")
    set_target_properties(DRMDrivers PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
    install(TARGETS DRMDrivers DESTINATION "${LIB_INSTALL_DIR}")
endif ()

if (MSVC)
    set_target_properties(DRMDrivers PROPERTIES
        COMPILE_PDB_NAME ${DRMDrivers_OUTPUT_NAME}
    )
endif ()

install(TARGETS DRMDrivers
        DESTINATION "${LIB_INSTALL_DIR}/"
)

configure_file(hbddrmdrivers.pc.in
        ${CMAKE_BINARY_DIR}/source/drmdrivers/hbddrmdrivers.pc @ONLY)
install(FILES "${DRMDrivers_PKGCONFIG_FILE}"
        DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
)

