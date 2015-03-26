#include "ntp.h"

#include "simplelink.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "mpexception.h"
#include "modutime.h"

STATIC mp_obj_t mod_ntp_gettimestamp(void) {
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

STATIC MP_DEFINE_CONST_FUN_OBJ_0(ntp_gettimestamp_obj, mod_ntp_gettimestamp);

STATIC mp_obj_t mod_ntp_getlocaltime(void) {
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

    return mp_obj_new_int_from_uint(ret - 946684800);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(ntp_getlocaltime_obj, mod_ntp_getlocaltime);

STATIC mp_obj_t mod_ntp_updatetime(void) {
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

    mod_struct_time tm;
    mod_time_seconds_since_2000_to_struct_time(ret, &tm);
    SlDateTime_t slTime = {0};
    slTime.sl_tm_year = tm.tm_year;
    slTime.sl_tm_mon = tm.tm_mon;
    slTime.sl_tm_day = tm.tm_mday;
    slTime.sl_tm_hour = tm.tm_hour;
    slTime.sl_tm_min = tm.tm_min;
    slTime.sl_tm_sec = tm.tm_sec;
    slTime.sl_tm_week_day = tm.tm_wday;
    slTime.sl_tm_year_day = tm.tm_yday;

    ret = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION, SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME, sizeof(SlDateTime_t), (uint8_t*)&slTime);

    if (ret != 0) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ret)));
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(ntp_updatetime_obj, mod_ntp_updatetime);


STATIC const mp_map_elem_t mp_module_ntp_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_ntp) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gettimestamp),         (mp_obj_t)&ntp_gettimestamp_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getlocaltime),         (mp_obj_t)&ntp_getlocaltime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_updatetime),         (mp_obj_t)&ntp_updatetime_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ntp_globals, mp_module_ntp_globals_table);

const mp_obj_module_t mp_module_ntp = {
    .base = { &mp_type_module },
    .name = MP_QSTR_ntp,
    .globals = (mp_obj_dict_t*)&mp_module_ntp_globals,
};
