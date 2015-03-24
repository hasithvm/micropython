
//#include <stdlib.h>

#include "crypto.h"
#include "py/nlr.h"
#include "py/runtime.h"



STATIC mp_obj_t crypto_hmacsha1(mp_obj_t *data_obj, mp_obj_t *key_obj) {
    // get the data and key
    mp_buffer_info_t data_buf;
    mp_get_buffer_raise(data_obj, &data_buf, MP_BUFFER_READ);

    mp_buffer_info_t key_buf;
    mp_get_buffer_raise(key_obj, &key_buf, MP_BUFFER_READ);

    vstr_t vstr;
    vstr_init_len(&vstr, HMACSHA1_DIGEST_SIZE);

    hmacsha1_hw(key_buf.buf, key_buf.len, data_buf.buf, data_buf.len, (uint8_t*)vstr.buf);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_2(crypto_hmacsha1_obj, crypto_hmacsha1);

STATIC const mp_map_elem_t mp_module_crypto_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_crypto) },
#if MICROPY_PY_UHASHLIB_HMACSHA1
    { MP_OBJ_NEW_QSTR(MP_QSTR_hmacsha1), (mp_obj_t)&crypto_hmacsha1_obj },
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_crypto_globals, mp_module_crypto_globals_table);

const mp_obj_module_t mp_module_crypto = {
    .base = { &mp_type_module },
    .name = MP_QSTR_crypto,
    .globals = (mp_obj_dict_t*)&mp_module_crypto_globals,
};
