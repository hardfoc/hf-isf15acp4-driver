/**
 * @file ports.cpp
 * @brief Instantiates the documented SPI port with the driver, so every member compiles in full.
 */
#include "my_spi.hpp"

template class isf15acp4::SmartDisplay<MySpi>;
