#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "py/runtime.h"
#include "bufhelper.h"
#include "inc/hw_types.h"
#include "inc/hw_mcspi.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "spi.h"
#include "mpexception.h"
#include "devicectl.h"
#include "proxima.h"
#include "pyspi.h"


// base typedef for Proxima/pyb SPI object
typedef struct {
    mp_obj_base_t base;
    uint mode;
    uint speed;
    uint slave;
    uint framesize;
    uint autoselect;
} proxima_spi_obj_t;


#define PROXIMA_SPI_MODE0   0
#define PROXIMA_SPI_MODE1   1
#define PROXIMA_SPI_MODE2   2
#define PROXIMA_SPI_MODE3   3




///////////////////////////Proxima bindings///////////////////////////////////////
STATIC int proxima_spi_init(uint mode, uint clkrate, uint framesize)
{
    uint spi_frame_size;
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);


    if (getControlStatus() == CONTROL_ASSERTED)
    {

        MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);
        
        MAP_PRCMPeripheralReset(PRCM_GSPI);
        MAP_SPIReset(GSPI_BASE);


        switch (framesize)
        {

            case 32:
                spi_frame_size = SPI_WL_32;
                break;
            case 16:
                spi_frame_size = SPI_WL_16;
                break;
            case 8:
                spi_frame_size = SPI_WL_8;
                break;
            default:
                spi_frame_size = SPI_WL_8;
                break;
        }

        // set up the SPI transfer parameters
        MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     clkrate,SPI_MODE_MASTER, mode,
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVEHIGH |
                     spi_frame_size));

        // setup GPIOs to be in SPI mode
        MAP_PinTypeSPI(IF_GSPI_MISO_PIN, PIN_MODE_7);
        MAP_PinTypeSPI(IF_GSPI_MOSI_PIN, PIN_MODE_7);
        MAP_PinTypeSPI(IF_GSPI_CLK_PIN, PIN_MODE_7);

        // setup SEL pins
        MAP_PinTypeGPIO(IF_GSPI_MISO_PIN, PIN_MODE_0, false);
        MAP_PinTypeGPIO(IF_GSPI_CLK_PIN, PIN_MODE_0, false);
        MAP_PinTypeGPIO(IF_GSPI_MOSI_PIN, PIN_MODE_0, false);

        // flip them into output mode
        MAP_PinDirModeSet(IF_GSPI_SEL0_PIN, PIN_DIR_MODE_OUT);
        MAP_PinDirModeSet(IF_GSPI_SEL1_PIN, PIN_DIR_MODE_OUT);
        MAP_PinDirModeSet(IF_GSPI_SEL2_PIN, PIN_DIR_MODE_OUT);
       
        // enable the SPI peripheral
        MAP_SPIEnable(GSPI_BASE);

        return 0;
    }
    else
    {
        return -1;
    }
}
void proxima_spi_select(uint slave)
{
        if (getControlStatus() == CONTROL_ASSERTED)
    {
        // assert the correct slave
        MAP_GPIOPinWrite(GPIOA0_BASE, _BV(0), slave & 0x01);
        MAP_GPIOPinWrite(GPIOA2_BASE, _BV(6), ((slave >> 1) & 0x01));
        MAP_GPIOPinWrite(GPIOA3_BASE, _BV(6), ((slave >> 2) & 0x01));
    }
}


void proxima_spi_transfer(uint slave, unsigned char* din,  unsigned char* dout, uint32_t len, uint select)
{
    if (getControlStatus() == CONTROL_ASSERTED)
    {
        if (select)
        {
            proxima_spi_select(slave);
        }
        MAP_SPITransfer(GSPI_BASE, dout,din, len, 0);
        if (select)
        {
            proxima_spi_select(0);
        }
    }
}




// shut down SPI and revert GPIOs to inputs (High-Z)
void proxima_spi_deinit()
{
    // shut down the SPI peripheral.
    MAP_PRCMPeripheralClkDisable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    // flip all the pins to GPIO mode, and default to inputs.
    MAP_PinTypeGPIO(IF_GSPI_MISO_PIN,PIN_MODE_0, false);
    MAP_PinTypeGPIO(IF_GSPI_CLK_PIN, PIN_MODE_0, false);
    MAP_PinTypeGPIO(IF_GSPI_MOSI_PIN, PIN_MODE_0, false);

    MAP_PinDirModeSet(IF_GSPI_MISO_PIN, PIN_DIR_MODE_IN);
    MAP_PinDirModeSet(IF_GSPI_MOSI_PIN, PIN_DIR_MODE_IN);
    MAP_PinDirModeSet(IF_GSPI_CLK_PIN, PIN_DIR_MODE_IN);

    MAP_PinDirModeSet(IF_GSPI_SEL0_PIN, PIN_DIR_MODE_IN);
    MAP_PinDirModeSet(IF_GSPI_SEL1_PIN, PIN_DIR_MODE_IN);
    MAP_PinDirModeSet(IF_GSPI_SEL2_PIN, PIN_DIR_MODE_IN);


}




/////////////////////////MicroPython bindings////////////////////////////////////

STATIC mp_obj_t proxima_mp_spi_read(mp_obj_t self_in, mp_obj_t obj_in)
{
    vstr_t v;
    mp_obj_t rv = pyb_buf_get_for_recv(obj_in, &v);
    proxima_spi_obj_t* spiobj = (proxima_spi_obj_t*)self_in;
    proxima_spi_transfer(spiobj->slave, NULL, (uint8_t*)v.buf, v.len, spiobj->autoselect);

    return (rv == NULL ? mp_obj_new_str_from_vstr(&mp_type_bytes, &v) : rv);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(proxima_mp_spi_read_obj, proxima_mp_spi_read);


STATIC mp_obj_t proxima_mp_spi_write(mp_obj_t self_in, mp_obj_t obj_in)
{
    mp_buffer_info_t buf;
    uint8_t tmp;
    proxima_spi_obj_t* spiobj = (proxima_spi_obj_t*)self_in;

    pyb_buf_get_for_send(obj_in, &buf, &tmp);
    proxima_spi_transfer(spiobj->slave, (uint8_t*)buf.buf,NULL, buf.len, spiobj->autoselect);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(proxima_mp_spi_write_obj, proxima_mp_spi_write);



// TODO: write symmetric transfer function
// STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(proxima_mp_spi_transfer_obj, 2, 3, proxima_mp_spi_transfer);

STATIC mp_obj_t proxima_mp_spi_init(mp_obj_t self_in)
{
    proxima_spi_obj_t* self = (proxima_spi_obj_t*) self_in;
    if (proxima_spi_init(self->mode, self->speed, self->framesize) != 0)
    {
        //TODO: raise an error here
        self = self;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(proxima_mp_spi_init_obj, proxima_mp_spi_init);





STATIC mp_obj_t proxima_mp_spi_deinit(mp_obj_t self_in) {
    proxima_spi_deinit();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(proxima_mp_spi_deinit_obj, proxima_mp_spi_deinit);



STATIC const mp_arg_t proxima_spi_ctor_args[] = {
    { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int  = PROXIMA_SPI_MODE0}},
    { MP_QSTR_speed, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int  = 5000000} },
    { MP_QSTR_framesize, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int  = 8} },
    { MP_QSTR_slave, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int  = 1} },
    { MP_QSTR_autoselect, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int  = 1} },
};

STATIC mp_obj_t proxima_mp_spi_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    


    proxima_spi_obj_t *self = m_new_obj_with_finaliser(proxima_spi_obj_t);
    self->base.type = &proxima_spi_type;

    // defaults to no-args.
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        mp_arg_val_t params[MP_ARRAY_SIZE(proxima_spi_ctor_args)];
        mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(proxima_spi_ctor_args), proxima_spi_ctor_args, params);
        self->mode = params[0].u_int;
        self->speed = params[1].u_int;
        self->framesize = params[2].u_int;
        self->slave = params[3].u_int;
        self->autoselect = params[4].u_int;
 



    return (mp_obj_t)self;
}


STATIC void proxima_mp_spi_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {

    proxima_spi_obj_t* spiobj = (proxima_spi_obj_t*) self_in;
    print(env, "(<Proxima SPI");
    print(env, ", baudrate=%u>", spiobj->speed);
    print(env, ", slave=%u, autoselect=%u", spiobj->slave, spiobj->autoselect);
    print(env, ", mode=%u>)", spiobj->mode);
}



STATIC const mp_map_elem_t proxima_spi_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_open),         (mp_obj_t)&proxima_mp_spi_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close),         (mp_obj_t)&proxima_mp_spi_deinit_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_send),         (mp_obj_t)&proxima_mp_spi_read_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_receive),         (mp_obj_t)&proxima_mp_spi_write_obj},
};

const mp_obj_type_t proxima_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = proxima_mp_spi_print,
    .make_new = proxima_mp_spi_make_new,
    .locals_dict = (mp_obj_t)&proxima_spi_locals_dict_table,
};