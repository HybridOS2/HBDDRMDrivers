if (NOT WTF_DIR)
    set(WTF_DIR "${CMAKE_SOURCE_DIR}/source/wtf")
endif ()
if (NOT DRMDRIVERS_DIR)
    set(DRMDRIVERS_DIR "${CMAKE_SOURCE_DIR}/source/drmdrivers")
endif ()
if (NOT THIRDPARTY_DIR)
    set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/source/third-party")
endif ()
if (NOT TOOLS_DIR)
    set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/tools")
endif ()

set(DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources")
set(WTF_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WTF")
set(DRMDrivers_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/DRMDrivers")
set(DRMDriversTestRunner_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/DRMDriversTestRunner")

set(FORWARDING_HEADERS_DIR ${DERIVED_SOURCES_DIR}/ForwardingHeaders)

set(bmalloc_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(WTF_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(DRMDrivers_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(DRMDrivers_PRIVATE_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
