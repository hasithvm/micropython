
#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "py/runtime.h"
#include "py/binary.h"
#include "mpexception.h"
#include "utils.h"
#include "gccollect.h"
#include "mperror.h"
#include "pyspi.h"
#include "proxima.h"



STATIC const mp_map_elem_t mp_module_proxima_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),            MP_OBJ_NEW_QSTR(MP_QSTR_Proxima) },
    // sub-objects
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPI),                (mp_obj_t)&proxima_spi_type },

};

STATIC MP_DEFINE_CONST_DICT(mp_module_proxima_globals, mp_module_proxima_globals_table);


const mp_obj_module_t mp_module_proxima = {
    .base = { &mp_type_module },
    .name = MP_QSTR_Proxima,
    .globals = (mp_obj_dict_t*)&mp_module_proxima_globals,
};