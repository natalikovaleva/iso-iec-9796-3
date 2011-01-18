unsigned char hexc_to_dec(unsigned char c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return c - '0';
    }
    else
        if ((c >= 'a') && (c <= 'f'))
        {
            return c - 'a' + 10;
        }
        else
            if ((c >= 'A') && (c <= 'F'))
            {
                return c - 'A' + 10;
            }
            else
                return 0;
}

unsigned char dec_to_hexc(unsigned char c)
{
    if (c < 10)
    {
        return '0' + c;
    }
    else
        if (c < 16)
        {
            return 'a' + c - 10;
        }
        else
            return 0;
}

static inline unsigned char invbits(unsigned char c)
{
    switch(c)
    {
        case 0: return 0;
        case 1: return 8;
        case 2: return 4;
        case 3: return 12;
        case 4: return 2;
        case 5: return 10;
        case 6: return 6;
        case 7: return 14;
        case 8: return 1;
        case 9: return 9;
        case 10: return 5;
        case 11: return 13;
        case 12: return 3;
        case 13: return 11;
        case 14: return 7;
        case 15: return 15;
        default:
            return 0;
    }
}

static inline unsigned int leading_zeros(unsigned char c)
{
    switch(c)
    {
        case 0: return 4;
        case 1: return 3;
        case 2: 
        case 3: return 2;
        case 4: 
        case 5: 
        case 6: 
        case 7: return 1;
        case 8: 
        case 9: 
        case 10: 
        case 11: 
        case 12: 
        case 13: 
        case 14: 
        case 15: return 0;
    }
    
    return 0;
}


#include <stdio.h>

unsigned char * mkle16b(unsigned char * dest, const unsigned char * src, unsigned int length)
{
    unsigned int i,j;
    unsigned int search_zeros = 0;
    unsigned char carry_mask = 0x0;
    unsigned char carry = 0;
    unsigned int shift = 0;
    
    for (i = 0, j=0; i<length; i++)
    {
        const unsigned char this = hexc_to_dec(src[length - i - 1]);

        search_zeros = search_zeros ? search_zeros : ( (this == 0) ? 0 : 1 );
        
        if (search_zeros)
        {
            if (search_zeros == 1)
            {
                search_zeros = 2;
                shift = leading_zeros(this);

                const unsigned char bits = invbits(this);
                
                carry_mask = 0xf >> (4 - shift);
                
                carry = bits >> shift;

                dest[length - i - 1] = 0;
                
                j++;
            }
            else
            {
                const unsigned char new_byte = invbits(this);

                if (shift)
                {
                    dest[j-1] = dec_to_hexc( carry | ((new_byte & carry_mask) << (4 - shift)));
                    carry = new_byte >> shift;
                }
                else
                {
                    dest[j-1] = dec_to_hexc( carry );
                    carry = new_byte;
                }
                
                
                j++;
            }
        }
    }

    return dest;
}

