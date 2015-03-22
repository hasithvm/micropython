#include "devicectl.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "proxima.h"
#include "gpio.h"


// internal variable to keep track of control pin status.
static unsigned int m_currentStatus=0xFF;

void initControl()
{
    // start the GPIO A0 peripheral 
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    
    // set up the GPIO for output
    MAP_PinTypeGPIO(IF_CONTROL_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA0_BASE, _BV(3), GPIO_DIR_MODE_OUT);
    
    // deassert it by default. Control is active LOW.
    MAP_GPIOPinWrite(GPIOA0_BASE, _BV(3), _BV(3));

    m_currentStatus = 0;
}

control_status_t assertControl()
{
    if (m_currentStatus != 0xFF)
    {
        MAP_GPIOPinWrite(GPIOA0_BASE, _BV(3), 0);
        m_currentStatus = 1;
    }
    return CONTROL_ASSERTED;
}

control_status_t getControlStatus()
{
    if (m_currentStatus==1)
    {
        return CONTROL_ASSERTED;
    }
    else if (m_currentStatus == 0)
    {
        return CONTROL_IDLE;
    }
    else
    {
        return CONTROL_ERR;
    }

}
control_status_t deassertControl()
{
    if (m_currentStatus != 0xFF)
    {
        MAP_GPIOPinWrite(GPIOA0_BASE, _BV(3), _BV(3));
        m_currentStatus = 0;
    }
    return CONTROL_IDLE;
}
