#pragma once

// OPEEngine
#include "OPEEngine_types.h"

const constexpr opee_size_t OPEEconfigCB_POOL_SZ = 512; ///< Size of combined callback storage pool in bytes.

const constexpr opee_size_t OPEEconfigCB_TASK_STACK_SZ =
        4095; ///< Stack size for callback execution task in words (according to freeRTOS port, on a 32-bit system (256words)(4bytes) == 1kB)
const constexpr opee_size_t OPEEconfigCB_TASK_PRIO = 7; ///< Priority of callback task, modify as needed, preferably kept as high priority as possible.

const constexpr opee_uint8_t OPEEconfigCB_QUEUE_SZ = 15; ///< Size of callback queue, maximum amount of callbacks that can be waiting for execution at any given time.

const constexpr opee_size_t OPEEconfigMAX_DATA_WATCH_CNT = 10; ///< Max amount of DataWatch objects that can be created.

const constexpr opee_size_t OPEEconfigMAX_DATA_WATCH_STK_SZ = 256; ///< Max size of a singular DataWatch stack.
