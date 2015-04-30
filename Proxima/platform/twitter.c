#include "twitter.h"

static char access_token[64];
static char access_secret[64];

void set_access_token(char* token, uint32_t token_len, char* secret, uint32_t secret_len) {
    memcpy(access_token, token, token_len);
    memcpy(access_secret, secret, secret_len);
}

void set_access_token(char* token, char* secret) {

}

void send_twitter_update(char* msg) {

}

void get_twitter_mentions(char* json_out, uint32_t buf_len) {

}

