# CompSec lab 1

> How to run?

1. Download [qemu disc image](https://drive.google.com/file/d/175TjUEESCko1JcbwoPieNLjGx4qgc2Si/view?usp=drive_link) and store it in this directory.
2. Open 2 terminal windows (or tabs).
3. Run `bash server.sh` in one of them and wait until `comp4634 login` text appears on screen (no need to login).
4. Run `bash client.sh` in the other (you can close it as soon as VSCode window opens).
5. Press `continue` when asked about fingerprint.
6. Enter password (`comp4634`) and wait a minute for system to load.
7. Enjoy :)

## Copy local project files to VM

```sh
scp -rpP 5555 localhost:proj1 user@localhost:/home/user
```

## Copy project files from VM to localhost

```sh
scp -rpP 5555 user@localhost:/home/user localhost:proj1
```
