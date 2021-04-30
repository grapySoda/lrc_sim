#ifndef _BITOPS_H_
#define _BITOPS_H_

int set_bit(int nr, unsigned long *addr)
{
        int mask, retval;

        addr += nr >> 5;
        mask = 1 << (nr & 0x1f);
        retval = (mask & *addr) != 0;
        *addr |= mask;
        return retval;
}

int clear_bit(int nr, unsigned long *addr)
{
        int mask, retval;

        addr += nr >> 5;
        mask = 1 << (nr & 0x1f);
        retval = (mask & *addr) != 0;
        *addr &= ~mask;
        return retval;
}

int test_bit(int nr, unsigned long *addr)
{
        int mask;

        addr += nr >> 5;
        mask = 1 << (nr & 0x1f);
        return ((mask & *addr) != 0);
}

#endif