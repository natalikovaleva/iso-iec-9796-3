#include "generic/convhex.h"

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

    dest[j-1] = dec_to_hexc( carry | (carry_mask << (4 - shift)));
    
    return dest;
}

