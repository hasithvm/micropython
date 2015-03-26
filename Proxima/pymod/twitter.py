import http
import crypto
import ntp
from ubinascii import b64encode
#import time

CONSUMER_KEY = "<consumer key>"
CONSUMER_SECRET = "<consumer secret>"

ACCESS_TOKEN = "<access token>"
ACCESS_TOKEN_SECRET = "<access secret>"

UPDATE_URL = "https://api.twitter.com/1.1/statuses/update.json"
#MENTIONS_URL = 

def update(msg):
    if len(msg) > 140:
        raise ValueError("Tweets can only be 140 characters long")

    time = str(ntp.gettimestamp())

    url = UPDATE_URL
    req = http.http_request(url, "POST")

    hash_vars = {}
    hash_vars['status'] = http.url_encode(msg, True)
    hash_vars['oauth_consumer_key'] = CONSUMER_KEY
    hash_vars['oauth_nonce'] = b64encode(crypto.sha1(time)).decode("utf-8").replace("+", "").replace("/","").replace("=","")
    hash_vars['oauth_signature_method'] = "HMAC-SHA1"
    hash_vars['oauth_timestamp'] = time #int(time.mktime(time.localtime()))
    hash_vars['oauth_token'] = ACCESS_TOKEN
    hash_vars['oauth_version'] = '1.0'

    sign = "&".join(["POST", http.url_encode(UPDATE_URL, True)])
    sign += "&" + http.url_encode("&".join(map(lambda x: x + "=" + str(hash_vars[x]), sorted(hash_vars))), True)

    key = CONSUMER_SECRET + "&" + ACCESS_TOKEN_SECRET

    hash_vars['oauth_signature'] = http.url_encode(b64encode(crypto.hmacsha1(sign, key)).decode("utf-8"), True)
    del hash_vars['status']

    oauth = "OAuth " + ", ".join(map(lambda x: x + "=\"" + hash_vars[x] + "\"", hash_vars))

    hash_vars = None
    key = None
    sign = None

    req.add_header("Authorization", oauth)
    req.add_header("Content-Type", "application/x-www-form-urlencoded")
    req.set_data("status=" + http.url_encode(msg, True))

    return req.request()
