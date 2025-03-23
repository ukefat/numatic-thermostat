/*
 * FreeRTOSMemory.cpp
 *
 *  Created on: Mar 11, 2025
 *      Author: kobru
 */
#include "FreeRTOSMemory.hpp"
#include "main.h"

std::unique_ptr<UBYTE[], FreeRTOSDeleter> make_unique_freertos_array(size_t size) {
    return std::unique_ptr<uint8_t[], FreeRTOSDeleter>(
        static_cast<UBYTE*>(pvPortMalloc(size))
    );
}
