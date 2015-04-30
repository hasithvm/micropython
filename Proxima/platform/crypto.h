#ifndef __HMAC_H__
#define __HMAC_H__

#define SHA1_DIGEST_SIZE 20
#define HMACSHA1_DIGEST_SIZE 20

#include <stdint.h>

void hmacsha1_hw(uint8_t* key, uint32_t key_len, uint8_t* data, uint32_t data_len, uint8_t* digest);
void sha1_hw(uint8_t* data, uint32_t data_len, uint8_t* digest);

#endif
