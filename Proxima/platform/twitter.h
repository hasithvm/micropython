#ifndef __TWITTER_H__
#define __TWITTER_H__

#define METHOD_GET 1
#define METHOD_POST 2

#define CONS_KEY "FN5d6anH4LPPYYbKTtVJdkFlo"
#define CONS_SECRET "H7ZiFffnQcx2InrUxEnliLvXsrs8JW3LUJJPGjxIb0uGOSugyx"

// #define ACCESS_TOKEN "3100897777-iuopcmmHUfR6rwWwSkePvXj1ZSDdXsDcFIPo56q"
// #define ACCESS_SECRET "wGKiBZmBS4cmJ2tC8pHDsxQGeIgQHDXw9ZYzLoAWyzfwc"

#define MENTION_URL "https://api.twitter.com/1.1/statuses/mentions_timeline.json"
#define MENTION_URL_FMT "GET&https%%3A%%2F%%2Fapi.twitter.com%%2F1.1%%2Fstatuses%%2Fmentions_timeline.json&count=%d&oauth_consumer_key=%s&oauth_nonce=%s&oauth_signature_method=HMAC-SHA1&oauth_timestamp=%d&oauth_token=%s&oauth_version=1.0"

#define TWEET_URL "https://api.twitter.com/1.1/statuses/update.json"
#define TWEET_URL_FMT "POST&https%%3A%%2F%%2Fapi.twitter.com%%2F1.1%%2Fstatuses%%2Fupdate.json&status=%s&oauth_consumer_key=%s&oauth_nonce=%s&oauth_signature_method=HMAC-SHA1&oauth_timestamp=%d&oauth_token=%s&oauth_version=1.0"

void set_access_token(char* token, char* secret);
void send_twitter_update(char* msg);
void get_twitter_mentions(char* json_out, uint32_t buf_len);

#endif
