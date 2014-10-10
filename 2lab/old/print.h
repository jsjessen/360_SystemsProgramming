//******** print.h ********

/* James Jessen            Group #1
 * 10918967                 Seq. #4
 *
 * CptS 360
 * Lab #2
 */


#ifndef __PRINT_H__
#define __PRINT_H__

typedef unsigned long u32;

int rpu(u32 x, u32 base);
int prints(char *s);
int printu(u32 x); // Unsigned Long
int printd(int x); // Signed Int
int printx(u32 x); // HEX
int myprintf(char *fmt, ...); // Formatted

#endif // __PRINT_H__
