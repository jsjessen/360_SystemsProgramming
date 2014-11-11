// James Jessen
// 10918967   
// 
// CptS 360

#include "util.h"

int test_bit(u8* buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    if (buf[byte] & (1 << bit))
        return 1;
    else
        return 0;
}

void set_bit(u8** buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    (*buf)[byte] |= (1 << bit);
}

void clear_bit(u8** buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    (*buf)[byte] &= ~(1 << bit);
}

int get_ideal_record_length(const int name_len)
{
    //     (4 * ((4 + 2 + 1 + 1 + name_len + 3) / 4))
    return (4 * ((      8       + name_len + 3) / 4)); // Ideal
}
