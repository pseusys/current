# COMP4634 (Fall 2024): Homework №1

> by Aleksandr Sergeev, student number: `21158862`.

## Problem 1: Jump Oriented Programming (JOP)

Register contents should be the following:

| Register | Value |
| --- | --- |
| ecx | `0x555555e8` |
| edx | `0x555555d0` |
| eip | `0x7fff2000` |

Memory contents should be the following:

| Address | Value |
| --- | --- |
| 0x555555d8 | `0x0000000055555588` |
| 0x555555e0 | any value |
| 0x555555e8 | `0xbb22555555ff2425` |
| 0x555555f0 | `0x0040ff7f00000000` |
| 0x555555f8 | any value |
| 0x55555600 | any value |

> NB! Since the contrary was not mentioned, I assume that addresses an range `0x555555d8 - 0x55555600` are executable.

With a configuration like that the following action sequence will happen:

1. Since current `eip` value is `0x7fff2000`, the gadget at `0x7fff2000` will be executed first, and after it is executed the following changes happen:

    - `edx` = `0x555555d8` (`0x555555d0` + `8`)
    - `eax` = `0x55555588` (`*0x555555d8` = `0x55555588`)
    - `eip` = `0x555555e8`

2. Then the code at `0x555555e8` will be executed.
    That code can be disassembled ([this webside](https://defuse.ca/online-x86-assembler.htm) was used for verification) to the following snippet:

    ```asm
    mov ebx, 0x55555522
    jmp QWORD PTR [0x7fff4000]
    ```

    After this code execution, the following changes happen:

    - `ebx` = `0x55555522`
    - `eip` = `0x7fff4000`

3. Then the gadget at `0x7fff4000` will be executed, and after it is executed the following changes happen:

    - `*0x55555588` = `0x55555522` (`[eax]` is set to `ebx`)

Just as it was required!

## Problem 2: Stack canaries

1. This is the code of the program I came up with:

    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <fcntl.h>
    #include <unistd.h>

    struct PtrBuff {
        char buffer[8];
        char* ptr;
    };

    void some_func(char* input) {
        struct PtrBuff v;
        v.ptr = (char*) 0xdeadbeefdeadbeef;

        strcpy(v.buffer, input);

        printf("Buffer: %s\n", v.buffer);
        printf("Pointer: %p\n", v.ptr);
    }

    int main(int argc, char *argv[]) {
        if (argc != 2) {
            printf("Usage: %s <input_file>\n", argv[0]);
            return 1;
        }

        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror("problem2: open error");
            exit(EXIT_FAILURE);
        }

        char buffer[1024];
        if (read(fd, buffer, sizeof(buffer)) < 0) {
            perror("problem2: read failed");
            exit(EXIT_FAILURE);
        }

        some_func(buffer);
        return 0;
    }
    ```

    Given a file with the following bytes content a buffer is overflown in this program and pointer is overwritten: `0xff 0xff 0xff 0xff 0xff 0xff 0xff 0xff 0xba, 0xba, 0xad, 0xde`.
    The output second line is: `0xde007fffdeadbaba`, meaning that the value of the pointer was corrupted and partially rewritten.
    Although this program doesn't allow channging all the bytes of the pointer at will, it still provides some space for attacking the stack variables within one structure.

2. In case of stack protection, control hijacking attack can be performed by e.g. corrupting a function pointer located on stack.
    In case there is a structure containing a function pointer in one of its fields (just as in example above), the pointer can be changed, so that another malicious function would be executed instead.

## Problem 3: Integer underflow vulnerability

The local buffer `buf` overflow should happen on line 15, for that `nlen + vlen + 1` should be greater than buffer length (`8264`).
Let us assume that both `hdr->nlen` and `hdr->vlen` fields have the same unsigned type (`uint32_t`).
In that case, computation of `nlen` can not be corrupted, all of the numbers used there are unsigned.
However, computation of `vlen` can be corrupter indeed!
Basically, it is intended that `vlen` becomes the **minimum** of `hdr->vlen` and `8192 - (nlen + 1)` (to fit into buffer).
However, inside of the `if` expression on line 12 the type of `8192 - (nlen + 1)` expression is **signed integer** (because it starts with an integer literal not marked with `u` to become unsigned).
That means, that if `nlen` is greater or equal than `8192`, the whole expression becomes unsigned and is always less than unsigned value of `vlen`.
Then, on the line 13, unsigned variable `vlen` is assigned with a signed value, which results in it having $2^32 - 1 - abs(8192 - (nlen + 1))$ value (very big number).
For this to happen, `nlen` could remain `8192`, so assignment on line 6 can be skipped.
As a result, we need the following header setup to perform the attack:

| Header field | Value | Explanation |
| --- | --- | --- |
| `hdr->nlen` | anything greater than `8192`, e.g. `10000` | While `hdr->nlen` is greater than `nlen`, value in `nlen` is equal to buffer size |
| `hdr->vlen` | anything positive, e.g. `10` | Literally any value in an unsigned field will be greater than negative number (in our case, `-1`) |
| `hdr->ndata` | byte string of length `8192`, e.g. `0x0 * 8192` | The buffer will be filled with this value on line 8 |
| `hdr->vdata` | byte string of length `71` and then attack payload | After `8193` bytes were written into buffer, only `71` empty byte remains there; on line 15 we will write $2^32 - 1$ bytes, so we can place attacking payload starting from `72`th position  |

## Problem 4: Privilege Escalation

This file can be executed on behalf of root user.
That could be an important security issue if `ping` utility contained any security bugs, however since we assumed it doesn't have any vulnerabilities in its implementation, no direct privelege escalation attacks can be performed.
Instead, some indirect attacks using elevated permissions for the `ping` code can be performed.
For example, some malicious code can be loaded into some of the shared libraries, scripts or environment used by the utility.
This attack can be prevented using one of the following mechanisms:

1. Disabling some of the system calls (e.g. `setuid`) in shared libraries.
2. Disabling non-root access to some of the directories containing shared libraries binaries and source code.
3. Rewriting utility to depend on as few external and shared code that can be attacked as possible.

I personally tried loading a malicious shared library containing `_init` function with `LD_PRELOAD` environment variable.
I was able to do it in user mode, the malicious library was taken into account, in root mode it wasn't.

However when I tried replacing the shared library that was to be loaded by the executable with the malicious shared library symlink, I was able to gain root access upon shared library loading.
I guess protecting shared library locations should protect system from this type of attacks normally.

## Problem 5: Android Isolation

Since Android system is based on Linux, it also extends Linux user permission rules.
Still, mobile devices usually work not in the same environment as normal Linux desktop or even server systems.
For instance, normally phones and tablets that run Android can support one user at a time only, so the whole user separation mechanism built into kernel becomes useless.
However, it could be re-used for another purpose to achieve better security.
If every app is run on behalf of a separate non-root user, it can limit all the read, write and execute access to all the files belonging to this app to itself only.
In a way, all the apps become "sandboxed" within the boundaries set up for the users running them (meaning both system file permissions, syscalls, private files, etc.).
As for the user groups, some shared files (e.g. image gallery) can be accessed by all users belonging to a specific group (e.g. group of apps that have a permission to view gallery).
Implementing this measure can protect private files belonging to an app from all the other apps even if they happen to learn exact path to these files somehow.
On the other hand, if all the apps run in separate VMs (as it was stated during lecture), having different user IDs inside of these VMs does not seem to introduce any additional protection measures.

## Problem 6: Reducing executable permissions

1. The core idea of the solution would be limiting access to `/usr/bin/passwd` tool to the new service `passwd` account and allowing other users use it instead of getting root permissions.
    The implementation of it could be done in a few steps:

    1. Setting intended permissions for `/usr/bin/passwd` utility (it will be owned by `passwd` user and all the other users can execute it on `passwd` users behalf):

        ```shell
        sudo chown passwd /usr/bin/passwd
        sudo chmod u+s /usr/bin/passwd
        ```

    2. Configure file permissions on `/etc/shadow` (only the `passwd` service account can write to it):

        ```shell
        sudo chown root:passwd /etc/shadow
        sudo chmod 640 /etc/shadow
        ```

    3. Modify `/usr/bin/passwd` code, checking that the currently logged in user can only change their own password.
        This can be done by ensuring the current user ID (retrieved with `getuid` call) equals to the ID of the user password is being changed for.

2. If there's a bug in the new version of `/usr/bin/passwd` utility code, even if getting root permissions directly would not be possible, an attacker could still manipulate the passwords.
    After getting access to all the users passwords, an attacker could just change root user password and do basically any damage to the system they would like.

3. Changing the user running the `/usr/bin/passwd` utility can indeed increase system security.
    Even though bugs in the utility itself could still lead to granting an attacker with root privileges, at least any user changing their password does not require root privileges anymore and can not damage system right away.
    Still, of course, everything related to user passwords management is a source of great risk to the whole system.

## Problem 7: Race conditions

1. A malicious actor can create a symlink named `file.dat` pointing to some important system file right after `stat` function is called (during `sleep` time).
    The impact of this attack depends on the file that symlink will point to, e.g. if the target will be `/etc/passwd`, it will get overwritten by `Hello world` that will cause large system disruptions.
2. Not that it will change anything: `stat` and `open` function calls are still not atomic and another process (creating the malicious symlink file) can still happen to execute between them.
    Even though that would still be harder and less likely, the attack can still happen.
3. Just as the hint suggested, the voulnerability can be prevented by using `open` function from `fcntl.h` library instead of `stat` and `fopen` functions.
    Instead of string mode it accepts open syscall flags as a second and file permission flags third arguments.
    So the safe alternative would be: `open("file.dat", O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)`.
    This call completely replecates flags set by `fopen` function [by default](https://man7.org/linux/man-pages/man3/fopen.3.html): `O_WRONLY | O_CREAT | O_TRUNC` for `"w"` open mode and `S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH` for permissions.
    The `O_EXCL` flag that has been added to the call introduces an atomic existance check and opens file only if it doesn't exists (otherwise failing with an appropriate error code).
    The safe code version would look like this:

    ```c
    int oflags = O_WRONLY | O_CREAT | O_TRUNC | O_EXCL;
    int fflags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    fp = open("file.dat", oflags, fflags);
    fprintf(fp, "Hello world");
    close(fp);
    ```

## Problem 8: Setuid

1. If there is a bug in `serve` function that allows arbitrary code execution, that code can run `setuid(0)` and gain privileged access to the serving machine.
    That could happen because `seteuid` only changes `EUID` value of the child process, leaving `RUID` and `SUID` unchanged (and equal to `0`).
    So since from the user mode `EUID` can only be changed to either `RUID` or `SUID`, it can indeed be restored to the root user ID.
2. The easiest change that could be made to prevent it is replacing code at line 7 to `setuid(100)`.
    In that case, since `setuid`is run in root mode, it will change all the ID values (`EUID`, `RUID` and `SUID`) forever with no possibility of restoration.

## Footnotes

Source code for problems 2 and 4 is included into submission as a zip archive.

1. For problem 2, code can be imported into VM (`problem2` directory) and run with command `python3 run_exploit.py`.
2. For problem 2, code can be imported into VM (`problem4` directory) and run with command `python3 run_exploit.py`.
    The python script is sensitive to `ATTACK_TYPE` environment variable, that can take one of the values `NONE`, `LD_PRELOAD` and `SYMLINK` for different attack type specification.
