/******** myprint.c ********/

/* James Jessen
 * 10918967
 *
 * CptS 360
 * Lab #2
 */

#include "myprint.h"

int rpu(u32 x, u32 base)
{
    static char *table = "0123456789ABCDEF";
    char c;

    if (x)
    {
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
        putchar(*cp++);
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
        putchar('0');
    else if (x < 0)
    {
        putchar('-');
        rpu(-x,10);
    }
    else
        rpu(x,10);
}

// Unsigned OCTAL
int printo(u32 x)
{
    putchar('0');

    if (x==0)
        putchar('0');
    else
        rpu(x,8);
}

// Unsigned HEX
int printx(u32 x)
{
    putchar('0');
    putchar('x');

    if (x==0)
        putchar('0');
    else
        rpu(x,16);
}

// Formatted Printing
int myprintf(char *fmt, ...)
{
    //                12    8      4      0
    // ... d | c | b | a | fmt | retPC | ebp | locals ...

    int *ebp = (int*)get_ebp();
    char *cp = fmt;

    // each int ptr increment = 4 bytes 
    // 12 / 4 = 3
    int *ip = ebp + 3;

    while(*cp)
    {
        if (*cp != '%')
        {
            // for each \n, spit out an extra \r
            if (putchar(*cp) == '\n')
                putchar('\r'); 
        }
        else
        {
            cp++; // bypass %
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

                case 'o': // OCTAL 
                    printo(*ip);
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