if (NOT TARGET DRMDrivers::DRMDrivers)
    if (NOT INTERNAL_BUILD)
        message(FATAL_ERROR "DRMDrivers::DRMDrivers target not found")
    endif ()

    # This should be moved to an if block if the Apple Mac/iOS build moves completely to CMake
    # Just assuming Windows for the moment
    add_library(DRMDrivers::DRMDrivers STATIC IMPORTED)
    set_target_properties(DRMDrivers::DRMDrivers PROPERTIES
        IMPORTED_LOCATION ${WEBKIT_LIBRARIES_LINK_DIR}/DRMDrivers${DEBUG_SUFFIX}.lib
    )
    set(DRMDrivers_PRIVATE_FRAMEWORK_HEADERS_DIR "${CMAKE_BINARY_DIR}/../include/private")
    target_include_directories(DRMDrivers::DRMDrivers INTERFACE
        ${DRMDrivers_PRIVATE_FRAMEWORK_HEADERS_DIR}
    )
endif ()
