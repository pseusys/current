# Part 2: Wireshark Packet Sniffing

The `wireshark` verison 4.2.2 was used.

1. For a closed port, response TCP packet carries no body, no sequence number and empty window size.
    It has only two flags set: `ACK` and `RST` (although conversation completeness section also mentions `SYN` flag, in the flags section itis not presen).
    Only one packet pair (request-response) is recorded.

2. For a filtered port, several request packets were sent (5 retries), but no responses received.
    The request packet (just like in case of closed ports), carries
no body, random srquence number and window size of 1024.
    It has only `SYN` flag set.

3. Once it detected an HTTP server, `nmap` sent multiple HTTP requests to it.
    First, a simple HTTP/1.0 `GET` request to the root was sent (it resulted in HTTP/1.1 200 response).
    Then HTTP/1.1 GET, two OPTION (one with `Access-Control-Request-Method: HEAD` and `origin: example.com` set and the other without), POST to `/sdk` endpoint (with some SOAP data), GET to `/.git/HEAD` endpoint, GET to `/nmaplowercheck1732021160` endpoint, GET to `/robots.txt` endpoint, also several PROPFIND and another OPTION messages.
    Finally, a POST request with XML encoded data (`<methodCall> <methodName>system.listMethods</methodName> <params></params> </methodCall>`) to the root was sent, likely for some kind of RPC detection and listing.  
    Some otherendpoints were queried with GET and OPTIONS later.
    Apparently, `nmap` just tries some common endpoints, because I could not find any of these URLs in the HTTP responses received before.
    Most of the endpoints specified were never found.

4. According to the [`nmap` documentation](https://nmap.org/book/man-os-detection.html), that is how `nmap` tries to detect HOST operational system:

    - Analyze TCP ISN (initial sequence number)
    - Detect TCP options support and order
    - Check intial window size
    - ... and also collect some other information not related to TCP directly.

    Then the information mentioned is compared to records in a special database and the prediction result is outputted.
    For the `scanme.nmap.org` host, the prediction is:

    - `Linux 4.15 - 5.19`: 97%
    - `Linux 5.0`: 94%
    - `Linux 5.0 - 5.14`: 94%
    - `OpenWrt 22.03 (Linux 5.10)`: 94%
    - `MikroTik RouterOS 7.2 - 7.5 (Linux 5.6.3)`: 94%
    - `HP P2000 G3 NAS device`: 92%
    - `OpenWrt 21.02 (Linux 5.4)`: 91%
    - `Linux 2.6.32`: 91%
    - `Linux 2.6.32 - 3.13`: 91%
    - `Linux 4.15`: 91%

    However, no exact matches were found, so we can not be 100% sure.
