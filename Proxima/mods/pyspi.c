#include <stdio.h>
#include <string.h>

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




// base typedef for Proxima/pyb SPI object
typedef struct {
    mp_obj_base_t base;
    int mode;
    uint baudrate;
} proxima_spi_obj_t;


#define PROXIMA_SPI_MODE0   0
#define PROXIMA_SPI_MODE1   1
#define PROXIMA_SPI_MODE2   2
#define PROXIMA_SPI_MODE3   3



STATIC const mp_arg_t proxima_spi_init_args[] = {
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int  = PROXIMA_SPI_MODE0} },
    { MP_QSTR_baudrate, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int  = 5000000} },
};

STATIC mp_obj_t proxima_spi_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {

    return mp_const_none;


}



STATIC mp_obj_t proxima_spi_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get spi mode
    mp_int_t spi_mode = mp_obj_get_int(args[0]);

    // make sure SPI mode is constrained
    if (spi_mode > 3 && spi_mode < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    // create and setup the object
    proxima_spi_obj_t *self = m_new_obj_with_finaliser(proxima_spi_obj_t);
    self->base.type = &proxima_spi_type;
    self->mode = spi_mode;
    return (mp_obj_t)self;
}


STATIC MP_DEFINE_CONST_FUN_OBJ_KW(proxima_spi_init_obj, 1, proxima_spi_init);


STATIC const mp_map_elem_t proxima_spi_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),         (mp_obj_t)&proxima_spi_init_obj },

};

const mp_obj_type_t proxima_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = proxima_spi_print,
    .make_new = proxima_spi_make_new,
    .locals_dict = (mp_obj_t)&proxima_spi_locals_dict,
};