import socket
import time


Xlate = {

	'a':'ɐ', 'b':'q', 'c':'ɔ', 'd':'p', 'e':'ə',
	'f':'ɟ', 'g':'ɓ', 'h':'ɥ', 'i':'\u0131', 'j':'ɾ',
	'k':'ʞ', 'l':'l', 'm':'ɯ', 'n':'u', 'o':'o',
	'p':'p', 'q':'q', 'r':'ɹ', 's':'s', 't':'ʇ',
	'u':'n', 'v':'ʌ', 'w':'ʍ', 'x':'x', 'y':'ʎ',
	'z':'z',

	'A':'∀', 'B':'B', 'C':'Ↄ', 'D':'◖', 'E':'Ǝ',
	'F':'Ⅎ', 'G':'⅁', 'H':'H', 'I':'I', 'J':'ſ',
	'K':'K', 'L':'⅂', 'M':'W', 'N':'ᴎ', 'O':'O',
	'P':'Ԁ', 'Q':'Ό', 'R':'ᴚ', 'S':'S', 'T':'⊥',
	'U':'∩', 'V':'ᴧ', 'W':'M', 'X':'X', 'Y':'⅄',
	'Z':'Z',

	'0':'0', '1':'1', '2':'0', '3':'Ɛ', '4':'ᔭ',
	'5':'5', '6':'9', '7':'Ɫ', '8':'8', '9':'0',

	'_':'¯', "'":',', ',':"'", '\\':'/', '/':'\\',
	'!':'¡', '?':'¿',
}

import sys

def upsidedown(msg):
	return ''.join([Xlate[c] if c in Xlate else c for c in reversed(msg)])


def url_encode(msg, full=False):
    msg = bytes(msg, "utf-8")
    out = ""

    for c in msg:
        if chr(c).lower() in "abcdefghijklmnopqrstuvwxyz0123456789-_.~":
            out += chr(c)
        else:
            out += "%{0}".format(hex(c)[2:].upper())

    return out


class http_request:

    HTTP = 1
    HTTPS = 2

    def __init__(self, url, method="GET"):
        if method in [ "GET", "POST" ]:
            self.method = method
        else:
            raise ValueError("Only 'GET' and 'POST' requests are supported")

        self.data = ""
        self.headers = {}
        self.add_header("Accept", "*/*")
        self.add_header("Connection", "close")
        self.add_header("User-Agent", "Proxima MicroPython v1.0")
        self.add_header("Content-Length", "0")

        if url is None:
            raise ValueError("URL must be valid")

        self.url = url

        if self.url[:7] == "http://":
            self.url = url[7:]
            self.protocol = http_request.HTTP

        elif self.url[:8] == "https://":
            self.url = url[8:]
            self.protocol = http_request.HTTPS

        if "/" in self.url:
            self.host = self.url[:self.url.find("/")]
            self.path = self.url[self.url.find("/"):]
        else:
            self.host = self.url
            self.path = "/"

        self.add_header("Host", self.host)

    def add_header(self, name, data):
        self.headers[name] = data

    def set_data(self, data):
        if self.method != 'GET':
            self.data = data
            self.add_header("Content-Length", str(len(data)))
        else:
            raise ValueError("Request data is not supported for 'GET' requests")

    def dump_headers(self):
        m = map(lambda header: header + ": " + self.headers[header], self.headers)
        return "\r\n".join(m)

    def dump(self):
        req = "{0} {1} HTTP/1.1\r\n{2}\r\n\r\n{3}".format(self.method, self.path, self.dump_headers(), self.data)
        print(req)


    def request(self):
        if self.protocol == http_request.HTTP:
            port = 80
            sock = socket.socket()
        else:
            port = 443
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_SSL)

        ip = socket.getaddrinfo(self.host, port)[0][4]

        req = "{0} {1} HTTP/1.1\r\n{2}\r\n\r\n{3}".format(self.method, self.path, self.dump_headers(), self.data)

        try:
            sock.connect(ip)
        except:
            print("Couldn't verify certificate, continuing...")

        sock.send(bytes(req, "UTF-8"))

        data = sock.recv(2048)

        sock.close()
        return data
