#===============================================================================
# ISF15ACP4 Driver - Root CMake
#===============================================================================

cmake_minimum_required(VERSION 3.16)

project(hf_isf15acp4_driver LANGUAGES CXX)

include(GNUInstallDirs)
# Installed layout mirrors the source tree, so the headers' "../src/*.ipp" includes resolve:
#   <prefix>/include/hf_isf15acp4/inc/*.hpp, isf15acp4_version.h
#   <prefix>/include/hf_isf15acp4/src/*.ipp
set(HF_ISF15ACP4_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/hf_isf15acp4")

include(${CMAKE_CURRENT_LIST_DIR}/cmake/hf_isf15acp4_build_settings.cmake)

add_library(${HF_ISF15ACP4_TARGET_NAME} STATIC ${HF_ISF15ACP4_SOURCE_FILES})
target_include_directories(${HF_ISF15ACP4_TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inc>
        $<BUILD_INTERFACE:${HF_ISF15ACP4_VERSION_HEADER_DIR}>
        $<INSTALL_INTERFACE:${HF_ISF15ACP4_INSTALL_INCLUDEDIR}/inc>
)
target_compile_features(${HF_ISF15ACP4_TARGET_NAME} PUBLIC cxx_std_20)

add_library(hf::isf15acp4 ALIAS ${HF_ISF15ACP4_TARGET_NAME})

set_target_properties(${HF_ISF15ACP4_TARGET_NAME} PROPERTIES
    EXPORT_NAME isf15acp4  # installed package target: hf::isf15acp4, the same name as the alias above
    VERSION ${HF_ISF15ACP4_VERSION}
    SOVERSION ${HF_ISF15ACP4_VERSION_MAJOR}
)

option(HF_ISF15ACP4_ENABLE_WARNINGS "Enable extra warnings for ISF15ACP4 driver" OFF)
if(HF_ISF15ACP4_ENABLE_WARNINGS)
    target_compile_options(${HF_ISF15ACP4_TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

#===============================================================================
# Optional: host unit tests and the compiled documentation snippets (tests/)
#===============================================================================
option(HF_ISF15ACP4_BUILD_TESTS "Build the host unit tests in tests/" OFF)
if(HF_ISF15ACP4_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

include(CMakePackageConfigHelpers)

install(TARGETS ${HF_ISF15ACP4_TARGET_NAME} EXPORT hf_isf15acp4Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${HF_ISF15ACP4_INSTALL_INCLUDEDIR}/inc
)

install(DIRECTORY inc/ DESTINATION ${HF_ISF15ACP4_INSTALL_INCLUDEDIR}/inc
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
install(DIRECTORY src/ DESTINATION ${HF_ISF15ACP4_INSTALL_INCLUDEDIR}/src
    FILES_MATCHING PATTERN "*.ipp")
install(FILES "${HF_ISF15ACP4_VERSION_HEADER}" DESTINATION ${HF_ISF15ACP4_INSTALL_INCLUDEDIR}/inc)

install(EXPORT hf_isf15acp4Targets
    FILE hf_isf15acp4Targets.cmake
    NAMESPACE hf::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_isf15acp4
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/hf_isf15acp4Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/hf_isf15acp4Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_isf15acp4
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/hf_isf15acp4ConfigVersion.cmake"
    VERSION ${HF_ISF15ACP4_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/hf_isf15acp4Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/hf_isf15acp4ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hf_isf15acp4
)
