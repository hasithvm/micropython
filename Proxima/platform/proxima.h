#ifndef __PROXIMA_H__
#define __PROXIMA_H__

#include "inc/hw_types.h"
#include "pin.h"

#define _BV(x)  (1<< x)


#define IF_CONTROL_PIN PIN_58
#define IF_GSPI_MISO_PIN PIN_06
#define IF_GSPI_MOSI_PIN PIN_07
#define IF_GSPI_CLK_PIN PIN_05

#define IF_GSPI_SEL0_PIN PIN_50
#define IF_GSPI_SEL1_PIN PIN_15
#define IF_GSPI_SEL2_PIN PIN_53

#define IF_GSPI_SDA_PIN PIN_03
#define IF_GSPI_SCL_PIN PIN_04

#endif
