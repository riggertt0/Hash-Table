#include "HashFunctions.h"
#include <assert.h>
#include <string.h>
#include <stdio.h> // ToDo: Delete include

hash_t ConstHash (const char* key)
{
    return 0;
}

hash_t LenHash (const char* key)
{
    assert (key);
    return strlen (key);
}

hash_t SumHash (const char* key)
{
    assert (key);
    hash_t result = 0;
    while (*key != 0)
    {
        result += *key;
        key++;
    }

    return result;
}

hash_t SumLenHash (const char* key)
{
    assert (key);

    hash_t len = LenHash (key);

    return (len == 0) ? 0 : SumHash (key) / len;
}

hash_t FirstCharHash (const char* key)
{
    assert (key);
    return *key;
}

hash_t RolHash (const char* key)
{
    assert (key);
    hash_t hash = 0;

    while (*key != 0)
    {
        hash = (hash << 1) + ((hash & MASK) >> ROL_ROR_SIZE) + *key; // shl (hash) + big byte + symbol
        key++;
    }

    return hash;
}

hash_t RorHash (const char* key)
{
    assert (key);
    hash_t hash = 0;

    while (*key != 0)
    {
        hash = (hash >> 1) + ((hash & 1) << ROL_ROR_SIZE) + *key; // shl (hash) + big byte + symbol
        key++;
    }

    return hash;
}

hash_t CRC32Hash (const char* key)
{
    assert (key);
    const hash_t polynomial  = 0x04C11DB7;
    const hash_t pol_old_bit = 1 << 26;
    hash_t hash = 0x0;

    while (*key)
    {
        for (int i_bit = 7; i_bit >= 0; i_bit--)
        {
            hash = (hash << 1) + (((*key) >> i_bit) & 1); // add 1 bit
            if (hash & pol_old_bit)
                hash ^= polynomial;
        }
        key++;
    }
    return hash;
}
