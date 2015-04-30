import http
import crypto
import ntp
from ubinascii import b64encode
import time

CONSUMER_KEY = "<key>"
CONSUMER_SECRET = "<secret>"

ACCESS_TOKEN = "<token>"
ACCESS_TOKEN_SECRET = "<token_secret>"

tweets = [ "Are you deliberately pushing my buttons?", "Proxima says hi from the SCE poster fair.", "My circuits are tingling...", "Why do people keep touching me?", "I felt that!", "Excuse me, you're squishing me.", "Please put me down...", "Can I help you?"]

UPDATE_URL = "https://api.twitter.com/1.1/statuses/update.json"
#MENTIONS_URL = 

_tidx = 0

def send_rnd(upside):
    global _tidx
    if upside:
        print(update(http.upsidedown(tweets[_tidx])))
    else:
        print(update(tweets[_tidx]))

    _tidx = (_tidx + 1) & 7

def update(msg):
    if len(msg) > 130:
        raise ValueError("Tweets can only be 140 characters long")

    timestamp = str(ntp.gettimestamp())
    tu = time.localtime(int(timestamp) - 946684800 - 14400)

    url = UPDATE_URL
    req = http.http_request(url, "POST")
    msg += " {0}:{1}:{2}".format(tu[3], tu[4], tu[5])

    hash_vars = {}
    hash_vars['status'] = http.url_encode(msg, True)
    hash_vars['oauth_consumer_key'] = CONSUMER_KEY
    hash_vars['oauth_nonce'] = b64encode(crypto.sha1(timestamp)).decode("utf-8").replace("+", "").replace("/","").replace("=","")
    hash_vars['oauth_signature_method'] = "HMAC-SHA1"
    hash_vars['oauth_timestamp'] = timestamp #int(time.mktime(time.localtime()))
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
