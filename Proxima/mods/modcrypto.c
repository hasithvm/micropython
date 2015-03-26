
#include "crypto.h"
#include "py/nlr.h"
#include "py/runtime.h"

STATIC mp_obj_t crypto_hmacsha1(mp_obj_t *data_obj, mp_obj_t *key_obj) {
    // get the data and key
    uint32_t oldlen;
    mp_buffer_info_t data_buf;
    mp_get_buffer_raise(data_obj, &data_buf, MP_BUFFER_READ);

    mp_buffer_info_t key_buf;
    mp_get_buffer_raise(key_obj, &key_buf, MP_BUFFER_READ);

    vstr_t vstr, tmp_key;
    vstr_init_len(&vstr, HMACSHA1_DIGEST_SIZE);

    if (key_buf.len > 64) {
        oldlen = key_buf.len;

        vstr_init_len(&tmp_key, SHA1_DIGEST_SIZE);

        sha1_hw(key_buf.buf, key_buf.len, (uint8_t*)tmp_key.buf);

        key_buf.len = oldlen;

        hmacsha1_hw((uint8_t*)tmp_key.buf, tmp_key.len, data_buf.buf, data_buf.len, (uint8_t*)vstr.buf);

    } else {
        hmacsha1_hw((uint8_t*)key_buf.buf, key_buf.len, data_buf.buf, data_buf.len, (uint8_t*)vstr.buf);
    }

    vstr_free(&tmp_key);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}

MP_DEFINE_CONST_FUN_OBJ_2(crypto_hmacsha1_obj, crypto_hmacsha1);

STATIC mp_obj_t crypto_sha1(mp_obj_t *data_obj) {
    mp_buffer_info_t data_buf;
    mp_get_buffer_raise(data_obj, &data_buf, MP_BUFFER_READ);

    vstr_t vstr;
    vstr_init_len(&vstr, SHA1_DIGEST_SIZE);

    sha1_hw(data_buf.buf, data_buf.len, (uint8_t*)vstr.buf);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}

MP_DEFINE_CONST_FUN_OBJ_1(crypto_sha1_obj, crypto_sha1);

STATIC const mp_map_elem_t mp_module_crypto_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_crypto) },
#if MICROPY_PY_UHASHLIB_HMACSHA1
    { MP_OBJ_NEW_QSTR(MP_QSTR_hmacsha1), (mp_obj_t)&crypto_hmacsha1_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sha1), (mp_obj_t)&crypto_sha1_obj },
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_crypto_globals, mp_module_crypto_globals_table);

const mp_obj_module_t mp_module_crypto = {
    .base = { &mp_type_module },
    .name = MP_QSTR_crypto,
    .globals = (mp_obj_dict_t*)&mp_module_crypto_globals,
};
