# =============================================================================
# hf-isf15acp4-driver: host unit tests
# =============================================================================
# Builds the driver headers against a recording SPI port and runs the tests
# with CTest. No hardware and no ESP-IDF needed:
#
#   cmake -S . -B build-host -DHF_ISF15ACP4_BUILD_TESTS=ON
#   cmake --build build-host
#   ctest --test-dir build-host --output-on-failure
# =============================================================================

add_executable(hf_isf15acp4_host_tests
  test_main.cpp
  test_driver.cpp
  test_gpio_errors.cpp
)

target_link_libraries(hf_isf15acp4_host_tests PRIVATE ${HF_ISF15ACP4_TARGET_NAME})
target_include_directories(hf_isf15acp4_host_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(hf_isf15acp4_host_tests PRIVATE cxx_std_20)
target_compile_options(hf_isf15acp4_host_tests PRIVATE -Wall -Wextra)

add_test(NAME hf_isf15acp4_host_tests COMMAND hf_isf15acp4_host_tests)

# The code shown on the documentation site (docs/ includes these files with pymdownx.snippets), built so a
# page cannot show code that does not compile against the current headers. Compiled, never linked or run.
add_library(hf_isf15acp4_doc_snippets OBJECT
  docs/quickstart.cpp
  docs/api_tasks.cpp
  docs/graphics.cpp
  docs/ports.cpp
)
target_link_libraries(hf_isf15acp4_doc_snippets PRIVATE ${HF_ISF15ACP4_TARGET_NAME})
target_include_directories(hf_isf15acp4_doc_snippets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/docs)
target_compile_features(hf_isf15acp4_doc_snippets PRIVATE cxx_std_20)
target_compile_options(hf_isf15acp4_doc_snippets PRIVATE -Wall -Wextra)

# The install rules: install the library into the build tree, then configure and build a find_package() consumer
# against that prefix only (tests/install_consumer/).
add_test(NAME hf_isf15acp4_install_package
  COMMAND ${CMAKE_COMMAND}
    -DBUILD_DIR=${PROJECT_BINARY_DIR}
    -DPREFIX=${CMAKE_CURRENT_BINARY_DIR}/install_prefix
    -DCONSUMER_SRC=${CMAKE_CURRENT_SOURCE_DIR}/install_consumer
    -DCONSUMER_BIN=${CMAKE_CURRENT_BINARY_DIR}/install_consumer
    -DCXX=${CMAKE_CXX_COMPILER}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/install_package_test.cmake
)
