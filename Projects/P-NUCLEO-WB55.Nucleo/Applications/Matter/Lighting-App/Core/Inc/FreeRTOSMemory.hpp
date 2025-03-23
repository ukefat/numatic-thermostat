/*
 * FreeRTOSMemory.hpp
 *
 *  Created on: Mar 10, 2025
 *      Author: kobru
 */

#pragma once

#include <memory>
#include "FreeRTOS.h"
#include "main.h"
// Custom deleter for FreeRTOS allocated memory
struct FreeRTOSDeleter {
    void operator()(void* ptr) const {
        vPortFree(ptr);
    }
};

std::unique_ptr<UBYTE[], FreeRTOSDeleter> make_unique_freertos_array(size_t size);


