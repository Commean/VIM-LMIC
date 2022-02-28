#include <iostream>
#include <lmic.h>

void printCred(u1_t *cred)
{
    size_t size = (sizeof cred / sizeof cred[0]);
    for (size_t i = 0; i < size; i++)
    {
        printf("%x ", cred[i]);
    }
    printf("\n");
}

const char *printBool(bool b)
{
    return b ? "✓" : "✗";
}

void printHex2(unsigned v)
{
    v &= 0xff;
    if (v < 16)
        printf("0");
    printf("%x", v);
}
