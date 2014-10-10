#include "print.h"
#include <stdio.h>

int main(void)
{
  printf("PRINTF\n");
  printf("this is a string\n");   // print the string
  printf("this %c is a %d test %x\n", 'a', 1234, 100);
  printf("\n\n");
  
  printf("MYPRINTF\n");
  myprintf("this is a string\n");   // print the string
  myprintf("this %c is a %d test %x\n", 'a', 1234, 100);
  return 0;
}
