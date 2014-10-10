//******** print.c ********

/* James Jessen            Group #1
 * 10918967                 Seq. #4
 *
 * CptS 360
 * Lab #2
 */

#include "print.h"

int rpu(u32 x, u32 base)
{
  static char *table = "0123456789ABCDEF";

  char c;
  if (x){
    c = table[x % base];
    rpu(x / base, base);
    putchar(c);
  }
} 

// String
int prints(char* s)
{
  char *cp = s;

  while(*cp)
    {
      putchar(*cp);
      cp++;
    }
}

// Unsigned Long
int printu(u32 x)
{
   if (x==0)
    putchar('0');
  else
    rpu(x,10);
}

// Signed Int
int printd(int x)
{
  if (x==0)
    {
      putchar('0');
    }
  else if (x < 0)
    {
      putchar('-');
      x = -x;
      rpu(x,10);
    }
  else
    {
      rpu(x,10);
    }
}

// Unsigned HEX
int printx(u32 x)
{
  if (x==0)
    putchar('0');
  else
    rpu(x,16);
}

// Formatted Printing
int myprintf(char *fmt, ...)
{
  int *ebp = (int*)get_ebp();
  char *cp = fmt;
  int *ip = ebp + 3; // each int increase = 4 bytes = 32 bits
  //                12    8      4      0
  // ... d | c | b | a | fmt | retPC | ebp | locals ...

  while(*cp)
    {
      if (*cp != '%')
	{
	  // for each \n, spit out an extra \r
	  if (putchar(*cp) == '\n')
	    {
	      putchar('\r'); 
	    }
	}
      else
	{

	  cp++;
	  switch (*cp) // *cp = char after %
	    {

	    case 'c': // char
	      putchar(*ip);
	      ip++;
	      break;

	    case 's': // string
	      prints((char*)(*ip));
	      ip++;	      
	      break;

	    case 'd': // int
	      printd(*ip);
	      ip++;
	      break;

	    case 'x': // HEX
	      printx(*ip);
	      ip++;
	      break;

	    case 'u': // unsigned int
	      printu(*ip);
	      ip++;
	      break;

	    case '%': // %% -> %
	      putchar('%');
	      break;
	      
	    default: // unknown specifier
	      putchar('%');
	      putchar(*cp);
	      break;

	    }// switch(*cp)

	}// if(%)

      cp++;

    } // while(cp)

} // myprintf()
