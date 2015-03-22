/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"

#define base64_enclen(a) (((((a)*4)/3) + 3) & 0xFFFFFFFC)
#define base64_declen(a) (((a)*3)/4)

#if MICROPY_PY_UBINASCII

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int base64decode (unsigned char *in, size_t inLen, char *out, size_t *outLen) {
    unsigned char *end = in + inLen;
    unsigned char iter = 0;
    size_t buf = 0, len = 0;

    while (in < end) {
        unsigned char c = d[*in++];

        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            /* If the buffer is full, split it into bytes */
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1; /* buffer overflow */
                *out++ = (buf >> 16) & 0xFF;
                *out++ = (buf >> 8) & 0xFF;
                *out++ = buf & 0xFF;
                buf = 0; iter = 0;

            }
        }
    }

    if (iter == 3) {
        if ((len += 2) > *outLen) return 1; /* buffer overflow */
        *out++ = (buf >> 10) & 0xFF;
        *out++ = (buf >> 2) & 0xFF;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1; /* buffer overflow */
        *out++ = (buf >> 4) & 0xFF;
    }

    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3)
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = ((uint32_t)data[x]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0

      if((x+1) < dataLength)
         n += ((uint32_t)data[x+1]) << 8;//parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0

      if((x+2) < dataLength)
         n += data[x+2];

      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;

      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];

      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }

      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }

   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0)
   {
      for (; padCount < 3; padCount++)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      }
   }
   if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   return 0;   /* indicate success */
}

STATIC mp_obj_t mod_binascii_hexlify(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

    vstr_t vstr;
    vstr_init_len(&vstr, bufinfo.len * 2);
    byte *in = bufinfo.buf, *out = (byte*)vstr.buf;
    for (mp_uint_t i = bufinfo.len; i--;) {
        byte d = (*in >> 4);
        if (d > 9) {
            d += 'a' - '9' - 1;
        }
        *out++ = d + '0';
        d = (*in++ & 0xf);
        if (d > 9) {
            d += 'a' - '9' - 1;
        }
        *out++ = d + '0';
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_binascii_hexlify_obj, 1, 2, mod_binascii_hexlify);

STATIC mp_obj_t mod_binascii_b64encode(mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    vstr_t vstr;
    vstr_init_len(&vstr, base64_enclen(bufinfo.len));

    base64encode(bufinfo.buf, bufinfo.len, vstr.buf, base64_enclen(bufinfo.len));

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_binascii_b64encode_obj, mod_binascii_b64encode);

STATIC mp_obj_t mod_binascii_b64decode(mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    size_t inlen = base64_declen(bufinfo.len);

    for(int i = bufinfo.len - 3; i < bufinfo.len; i++) {
        if (((char*)bufinfo.buf)[i] == '=') {
            inlen--;
        }
    }

    vstr_t vstr;
    vstr_init_len(&vstr, inlen);

    base64decode(bufinfo.buf, bufinfo.len, vstr.buf, &inlen);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_binascii_b64decode_obj, mod_binascii_b64decode);

STATIC const mp_map_elem_t mp_module_binascii_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ubinascii) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hexlify), (mp_obj_t)&mod_binascii_hexlify_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_unhexlify), (mp_obj_t)&mod_binascii_unhexlify_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_a2b_base64), (mp_obj_t)&mod_binascii_a2b_base64_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_b2a_base64), (mp_obj_t)&mod_binascii_b2a_base64_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_b64encode), (mp_obj_t)&mod_binascii_b64encode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_b64decode), (mp_obj_t)&mod_binascii_b64decode_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_binascii_globals, mp_module_binascii_globals_table);

const mp_obj_module_t mp_module_ubinascii = {
    .base = { &mp_type_module },
    .name = MP_QSTR_ubinascii,
    .globals = (mp_obj_dict_t*)&mp_module_binascii_globals,
};

#endif //MICROPY_PY_UBINASCII
