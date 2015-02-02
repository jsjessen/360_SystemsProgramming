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
    while(*s)
        putchar(*s++);
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
int myprintf(const char *fmt, ...)
{
    //                12    8      4      0
    // ... d | c | b | a | fmt | retPC | ebp | locals ...
    
    va_list args;
    va_start(args, fmt); // Initialize the argument list

    while(*fmt != '\0')
    {
        if(*fmt != '%')
        {
            // for each \n, spit out an extra \r
            if(putchar(*fmt) == '\n')
                putchar('\r'); 
        }
        else
        {
            fmt++; // bypass %
            switch(*fmt) // *fmt = char after %
            {
                case 'c': // char
                    putchar(va_arg(args, int)); //compiler said use int
                    break;

                case 's': // string
                    prints(va_arg(args, char*));
                    break;

                case 'd': // int
                    printd(va_arg(args, int));
                    break;

                case 'o': // OCTAL 
                    printo(va_arg(args, u32));
                    break;

                case 'x': // HEX
                    printx(va_arg(args, u32));
                    break;

                case 'u': // unsigned int
                    printu(va_arg(args, u32));
                    break;

                case '%': // %% -> %
                    putchar('%');
                    break;

                default: // unknown specifier
                    putchar('%');
                    putchar(*fmt);
                    break;

            }// switch(*fmt)
        }// if(%)

        fmt++;

    } // while(fmt)
} // myprintf()
