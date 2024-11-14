# COMP4634 (Fall 2024): Homework №1

> by Aleksandr Sergeev, student number: `21158862`.

## Problem 1: Same Origin Policy

The triplet `(protocol, domain, port)` defines a socket used for communication with the process hosting the webpage a user is connecting to.
If the origin would be defined by the `domain` only, that would allow performing attacks from the same public domain, but different port.
For example, in that case I would be able to launch a malicious process within `connect.ust.hk` private network and then read (with plain `fetch()` method) private files on other pages hosted within the same domain and steal other students private data.
That would be possible because server would only check domain name and not port and would consider my maliciouss process launched on an arbitrary port to have the same origin as the default `HTTP` port number (443).
Another example would be reading private data of the other users on a shared platform that allows user script execution, like `https://jsfiddle.net`.

## Problem 2: Cross Site Script Inclusion (XSSI) Attacks

## Problem 3: HTML Canvas Element

Embedding objects into HTML normally is regulated by way more relaxed rules then direct HTTP requests made by JavaScript code.
In case an image containing private user data can be embedded into another potentially malicious website HTML `<canvas>` element and then its pixels can be read by JavaScript code, that is a security vulnerability itself.
In that case malicious JavaScript can reconstruct private user images pixel-by-pixel and then analyze the data.

Several defence measures can be used to prevent this attack.
The easiest method would be restricting `GetImageData` to only be able to read pixels of canvas belonging to the same origin as the script itself.
If the domain itself is not malicious and only script can not be trusted, setting correct CSP header would be a way to disallow any data from other, potentially private, origins.
Also in case the image is loaded via `<img>` element, special `tainted` mechanism normally prevents JavaScript from reading `<canvas>` data.
Finally, as a general proposal in the future, additional cross-origin control rules can be applied to any elements that support embedding (e.g. in a new HTML version).
That would boost security of private elements (such as imags, videos, etc.) as well as commonly shared elements (as fonts or scripts) that are for some reason are not intended for sharing.

Once again, the security problem is straightforward.
If a malicious script places transparent `<canvas>` over sensitive parts of the same website (or the whole window), an attacker can actually make screenshots of any forms, passwords and any othe private elements.
Since the defences described above cover cross-origin-related issues, they will not help defending against an attack on some parts of the same website.

The easiest defence would be returning pixel values from the `<canvas>` itself instead of actual pixel value.
If the RGBA pixel value returned for a fully-transparent pixel would always be (0, 0, 0, 0.0) then the attack will always fail.
Another option would be dynamically replacing an actual transparent pixel value inside of the `GetImageData` with black color or just throwing an exception when called on partly-transparent `<canvas>`.

## Problem 4: CSRF Defenses



## Problem 5: Content Security Policies
