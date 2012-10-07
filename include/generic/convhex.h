#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    inline unsigned char invbits(unsigned char c)
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

    inline unsigned int leading_zeros(unsigned char c)
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

    inline unsigned char hexc_to_dec(unsigned char c)
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

    inline unsigned char dec_to_hexc(unsigned char c)
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

    
#ifdef __cplusplus
}
#endif
