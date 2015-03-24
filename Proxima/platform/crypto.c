#include "crypto.h"

#include <string.h>
#include <stdbool.h>

#include "rom.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "shamd5.h"

void hmacsha1_hw(uint8_t* key, uint32_t key_len, uint8_t* data, uint32_t data_len, uint8_t* digest) {
    uint8_t key_filled[64];

    memset(key_filled, 0, sizeof(key_filled));
    memcpy(key_filled, key, key_len);

    MAP_SHAMD5ConfigSet(SHAMD5_BASE, SHAMD5_ALGO_HMAC_SHA1);

    MAP_SHAMD5HMACKeySet(SHAMD5_BASE, key_filled);
    MAP_SHAMD5HMACProcess(SHAMD5_BASE, data, data_len, digest);
}

