/******** myprint.h ********/

/* James Jessen
 * 10918967
 *
 * CptS 360
 * Lab #2
 */

#ifndef __MY_PRINT_H__
#define __MY_PRINT_H__

typedef unsigned long u32;

int rpu(u32 x, u32 base);
int prints(char *s);            // String
int printu(u32 x);              // Unsigned Long
int printd(int x);              // Signed Int
int printo(u32 x);              // OCTAL
int printx(u32 x);              // HEX
int myprintf(char *fmt, ...);   // Formatted

#endif // __MY_PRINT_H__