# COMP5621 Computer Networks - Homework I

> by Aleksandr Sergeev, student number: `21158862`.

## Question 1

1. Let's calculate file transmission time over the given link.
    It's important to recall that there are 8192 gigabits in one terabyte.
    So the total transmission time would be equal `10000 * 8192 / 100` seconds, that is approximately 9.5 days.
    In this case I would obviously prefer FedEx.
2. Let's recalculate transmission time.
    Now it becomes `1000 * 8192 / 100` seconds, that is almost 23 hours.
    Given my and my recipients' travel time to FedEx office, I would prefer file transfer over internet.

## Question 2

1. The server successfully found the document (`HTTP/1.1 200 OK` status code) at `Fri, 28 Sep 2012 06:16:38 GMT` (`Date` field).
2. The document was last modified on `Fri, 31 Aug 2013 19:42:29 GMT` (`Last-Modified` field).
    The fact that last modification date is earlier than response date puzzled me a lot and honestly I could find no explanation of it, but for server misconfiguration or some error.
3. The server likely considers the request to be issued by a new user, because the response contains `Set-Cookie` field.
    The cookie might be useful for identification of the requests coming from the same user in the future (or contain some other session state information).
4. According to the [Wikipedia page](https://wikipedia.org/wiki/HTTP_cookie), cookies are small information pieces stored in user's browsers by various websites.
    The cookies are in general useful for storing session information between several HTTP requests (such as login, previous activity or data required for further browsing).
    Tracking cookies are used for monitoring user behavior over a long period of time, potentionally on several websites.
    They are often set by advertising agencies and used for showing user targeted ads.
5. According to the [Wikipedia article](https://wikipedia.org/wiki/Chunked_transfer_encoding), chunked transfer encoding is a special mechanism in HTTP/1.1, especially suitable for dynamically generated content transferring.
    The data is split into several independent chunks, each of them is sent upon generation and is preceded by its size in hexadecimal.
    Zero-length chunk means end of transmission.
    This response carries a chunk of data (according to `Transfer-Encoding: chunked` header line), and the object itself is `0x3fb0` (`16304`) bytes long.
6. The first 10 bytes of the response body are: "0x3C 0x21 0x44 0x4F 0x43 0x54 0x59 0x50 0x45 0x20" (<\!DOCTYPE ).
    According to the header line `Connection: Keep-Alive`, the server did agree for a persistent connection.
7. According to the [Mozilla developer website](https://developer.mozilla.org/docs/Web/HTTP/Headers/Vary), `Vary` header field can be used for storing information about what request header fields were used for the returned document generation.
    This information can be used by caches to determine for what requests exactly this response can be reused.
    For instance, the server response can be different for different values of `Accept`, `Accept-CH`, `Accept-Language` or `Accept-Encoding` request fields (this is called server-driven content negotiation).
    Another example would be variation based on user device, `User-Agent` request header.
    In this header, we can see server-driven content negotiation on `Accept-Encoding` request field.
8. According to the [Mozilla developer website](https://developer.mozilla.org/docs/Web/HTTP/Headers/ETag), `ETag` header field specifies some specific version of the object received.
    It simplifies caching process, requiring server to generate a new `ETag` for every new document verision.
    In the header above we can see a `ETag` with value of `2403f5-5797-4c894fe021a42`.

## Question 3

1. According to this [Cloudflare article](https://www.cloudflare.com/learning/dns/dns-records/dns-aaaa-record/) `AAAA` record is basically an IPv6 version of `A` record (and it makes some sense since IPv6 is exactly 4 times longer than IPv4).
    Just like `A` record, it maps domain names to IP addresses, IPv6 addresses in this case.
    DNS `PTR` record (according to this [Cloudflare article](https://www.cloudflare.com/learning/dns/dns-records/dns-ptr-record/)) is used for reverse DNS lookups.
    Sometimes it is quite useful to not only check IPs associated with given domain names, but also domain names associated with certain IPs (for example to verify an email address authority).
2. According to this [Cloudflare article](https://www.cloudflare.com/learning/dns/dns-cache-poisoning/), DNS cache poisoning is a cyber attack, that includes faking a DNS response, resulting in storing a wrong response in DNS cache.
    Future DNS requests will hit the cache, resulting in multiple users using false IP (often the one of a malicious webpage) for a certain website.
    One of the methods to prevent DNS cache poisoning is `DNSSEC` protocol, that verifies response authenticity and integrity.
    Although it's not wide-spread yet, it can help preventing attacks on DNS query almost like HTTPS helps preventing attacks on HTTP.
