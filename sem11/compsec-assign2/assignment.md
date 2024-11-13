# COMP4634 (Fall 2024): Homework №1

> by Aleksandr Sergeev, student number: `21158862`.

## Problem 1: Same Origin Policy

The triplet `(protocol, domain, port)` defines a socket used for communication with the process hosting the webpage a user is connecting to.
If the origin would be defined by the `domain` only, that would allow performing attacks from the same public domain, but different port.
For example, in that case I would be able to launch a malicious process within `connect.ust.hk` private network and then read (with plain `fetch()` method) private files on other pages hosted within the same domain and steal other students private data.
That would be possible because server would only check domain name and not port and would consider my maliciouss process launched on an arbitrary port to have the same origin as the default `HTTP` port number (443).
Another example would be reading private data of the other users on a shared platform that allows user script execution, like `https://jsfiddle.net`.

## Problem 2: Cross Site Script Inclusion (XSSI) Attacks
