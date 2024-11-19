# Part 1: Nmap Port Scanning

1. I decided not to install `nmap` locally, instead I used a docker image with the following command: `docker run --rm -it instrumentisto/nmap -sS -A -T4 -p1-65535 ${SCAN_HOST}`.  
    Where:

    - `-sS` sets TCP SYN scan
    - `-A` enables OS detection, version detection, script scanning, and traceroute
    - `-T4` sets quick scan
    - `-p1-65535` sets port range to scan (from 1 to 65535 means all the ports)
    - `SCAN_HOST` envirnment variable was set to `scanme.nmap.org`

2. IP addresses of `scanme.nmap.org` are `45.33.32.156` (for IPv4) and `2600:3c01::f03c:91ff:fe18:bb2f` (for IPv6, was not scanned).

3. The open ports are:

    - `22/tcp` ssh
    - `25/tcp` smtp
    - `80/tcp` http
    - `139/tcp` netbios-ssn
    - `161/tcp` snmp
    - `162/tcp` snmptrap
    - `593/tcp` http-rpc-epmap
    - `1434/tcp` ms-sql-m
    - `2745/tcp` urbisnet
    - `3127/tcp` ctx-bridge
    - `4444/tcp` krb524
    - `5104/tcp` tinymessage
    - `5112/tcp` pm-cmdsvr
    - `5136/tcp` minotaur-sa
    - `5152/tcp` sde-discovery
    - `5157/tcp` mediat
    - `5190/tcp` aol
    - `5195/tcp` ampl-lic
    - `6129/tcp` unknown
    - `7547/tcp` cwmp
    - `9929/tcp` nping-echo
    - `10168/tcp` unknown
    - `31337/tcp` tcpwrapped

4. The server is an `Apache` HTTP server, it runs on `80` TCP port, the server version is: `Apache httpd 2.4.7 ((Ubuntu))`.
