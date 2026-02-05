#ifndef __IC1914_H
#define __IC1914_H

#include <stdint.h>
#include "color.h"
#include "port_macros.h"

__attribute__((section(".text"))) void ic1914_update(Color_RGB_t *color_buf, uint16_t len, uint16_t full_len);

#endif

