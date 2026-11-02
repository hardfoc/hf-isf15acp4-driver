# Consumer of the installed package: built by the install_package test in tests/CMakeLists.txt to check that
# find_package(hf_isf15acp4) gives a target whose headers (and the .ipp files they include) compile and link.
cmake_minimum_required(VERSION 3.16)
project(hf_isf15acp4_install_consumer LANGUAGES CXX)

find_package(hf_isf15acp4 REQUIRED CONFIG)

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE hf::isf15acp4)
