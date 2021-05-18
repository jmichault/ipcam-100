#!/usr/bin/python
r"""
Taken from https://github.com/plone/plone.session/blob/master/plone/session/tktauth.py
It needed some modifications to work properly:
- The IP must be encoded as a string (plone was using 4-bytes). The reason mod_authn_tkt
does this is because an IP address might be IPv4 or it might be IPv6. An IPv6 address
does not fit into 4 packed bytes (which is the size of an IPv4 address)
- The first digest (digest0) used to calculate the final digest in mod_auth_tkt_digest()
must be the 16-bytes array while the original used the hex-string
"""

from struct import pack

import hashlib
import hmac
import six
import time
import sys
import os

def safe_encode(value, encoding='utf-8'):
    """Convert unicode to the specified encoding.

    copied from Products.CMFPlone.utils b/c this package does not depend on it
    """
    if isinstance(value, six.text_type):
        value = value.encode(encoding)
    return value

def mod_auth_tkt_digest(secret, data1, data2):
    # data1: addr + timestamp
    # data2: userid+token_list+user_data
    #digest0 = hashlib.md5(data1 + secret + data2).hexdigest() # THIS line that was modified to work with lighttpd mod_authn_tkt
    # The original code was using the hex-string while mod_authn_tkt uses the bytes
    digest0 = hashlib.md5(data1 + secret + data2).digest()
    if not six.PY2:
        # In Python 3 hashlib.md5(value).hexdigest() wants a bites value
        # and returns text
        digest0 = safe_encode(digest0)
    digest = hashlib.md5(digest0 + secret).hexdigest()
    return digest

def createTicket(secret, userid, tokens=(), user_data='', ip='0.0.0.0',
                 timestamp=None, encoding='utf-8', mod_auth_tkt=False):
    """
    By default, use a more compatible
    """
    if timestamp is None:
        timestamp = int(time.time())
    secret = safe_encode(secret)
    userid = safe_encode(userid)
    tokens = [safe_encode(t) for t in tokens]
    user_data = safe_encode(user_data)

    token_list = b','.join(tokens)
    ##data1 = inet_aton(ip)[:4] + pack('!I', timestamp) # THIS line that was modified to work with lighttpd mod_authn_tkt
    # Notice the difference is that 'ip' is used as a string
    data1 = safe_encode(ip) + pack('!I', timestamp)
    data2 = b''.join((userid, token_list, user_data))
    if mod_auth_tkt:
        digest = mod_auth_tkt_digest(secret, data1, data2)
    else:
        # a sha256 digest is the same length as an md5 hexdigest
        digest = hmac.new(secret, data1 + data2, hashlib.sha256).digest()

    if not isinstance(digest, six.binary_type):
        digest = digest.encode()

    # digest + timestamp as an eight character hexadecimal + userid + !
    ticket = b'%s%08x%s!' % (digest, timestamp, userid)
    if tokens:
        ticket += token_list + b'!'
    ticket += user_data

    return ticket

if __name__ == '__main__':
    SECRET = 'abcdefghijklmnopqrstuvwxyz0123456789'
    userid = "UserID"
    tokens = ("")
    user_data = "HOLA"
    ip_addr = os.environ["REMOTE_ADDR"]
    tkt = createTicket(SECRET, userid, tokens=tokens, user_data=user_data, ip=ip_addr, timestamp=1, mod_auth_tkt=True)
    from six.moves import http_cookies
    import binascii
    cookie = http_cookies.SimpleCookie()
    cookie['auth_tkt'] = binascii.b2a_base64(tkt).strip().decode()

    sys.stdout.write("Status: 200 OK\r\n");
    sys.stdout.write("Content-type: text/html\r\n");
    sys.stdout.write("%s\r\n\r\n" % (cookie));
    print("<!doctype html>");
    print("<html lang=\"en\">");
    print("<head>");
    print("<meta charset=\"utf-8\">");
    print("<title>My Auto Login CGI</title>");
    print("</head>");
    print("<body>");
    print("<p>You have been logged in! Try accessing the <a href=\"/protected.html\">Secrets</a> now<p>");
    print("<p>This <b>Python</b> CGI should check username and password in whichever way and then generate the cookie, but for this example it automatically lets you in<p>");
    print("<p>The auth_tkt Cookie should look like: %s<p>" % (cookie['auth_tkt']));
    print("<p>Press F12 to check it out in the dev tools of your browser<p>");
    print("<p>If you modify or delete it, you'd be logged off<p>");
    print("</body>");
