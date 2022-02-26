#include <iostream>

void printCred(u1_t *cred)
{
    size_t size = (sizeof cred / sizeof cred[0]);
    for (size_t i = 0; i < size; i++)
    {
        printf("%x ", cred[i]);
    }
    printf("\n");
}