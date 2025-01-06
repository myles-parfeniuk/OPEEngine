#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"
#include "stdint.h"

/* Override UNITY_OUTPUT_CHAR */
#define UNITY_OUTPUT_CHAR(a) Test_putchar(a)

void Test_putchar(int c);

#ifdef __cplusplus
}
#endif