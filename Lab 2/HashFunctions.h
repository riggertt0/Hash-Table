typedef unsigned long long hash_t;

const int ROL_ROR_SIZE = 8 * sizeof (hash_t) - 1;
const hash_t MASK = 1ULL << ROL_ROR_SIZE;

hash_t ConstHash     (const char* key);
hash_t LenHash       (const char* key);
hash_t SumHash       (const char* key);
hash_t SumLenHash    (const char* key);
hash_t FirstCharHash (const char* key);
extern "C"
{
    hash_t RolHash       (const char* key);
    hash_t RorHash       (const char* key);
    hash_t CRC32Hash     (const char* key);
}
