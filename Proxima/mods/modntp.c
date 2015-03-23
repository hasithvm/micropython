#include "ntp.h"

#include "py/mpstate.h"
#include "py/runtime.h"
#include "mpexception.h"

STATIC mp_obj_t mod_ntp_gettime(void) {
    int err = 0;
    uint32_t ret = get_ntp_utc(&err);

    switch(ret) {
        case NTP_ERR_NO_NIC:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_resource_not_avaliable));
            break;
        case NTP_ERR_SOCKET:
        case NTP_ERR_SEND:
        case NTP_ERR_RECV:
        case NTP_ERR_LOOKUP:
        case NTP_ERR_SET_TIMEOUT:
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ret)));
    }

    return mp_obj_new_int_from_uint(ret);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(ntp_gettime_obj, mod_ntp_gettime);

STATIC const mp_map_elem_t mp_module_ntp_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_ntp) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gettime),         (mp_obj_t)&ntp_gettime_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ntp_globals, mp_module_ntp_globals_table);

const mp_obj_module_t mp_module_ntp = {
    .base = { &mp_type_module },
    .name = MP_QSTR_ntp,
    .globals = (mp_obj_dict_t*)&mp_module_ntp_globals,
};
