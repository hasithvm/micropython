
#ifndef HMACSHA1_H
#define HMACSHA1_H

#define HMACSHA1_DIGEST_SIZE 20

void hmac_sha1(
                unsigned char *key,
                int key_length,
                const unsigned char *data,
                int data_length,
                unsigned char *digest
                );

#endif

