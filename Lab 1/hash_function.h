#include "header.h"
#include "hash_functions.h"
#include "hash_table.h"

typedef CRC32Hash HASH;

int main()
{
    std::ifstream in;
    in.open(VOCABULARY);

    auto table = hash_table<char*, HASH>(TABLE_SIZE, HASH());
    char* words = new char[N_WORDS * WORD_SIZE]();

    for (int i = 0; i < N_WORDS * WORD_SIZE; i += WORD_SIZE)
    {
        in >> (words + i);
        table.add(words + i);
    }

    std::ofstream out;
    out.open(LOG_PATH);
    table.dump_lists_lens(out);

    in.close();
    out.close();

    delete[] words;

    return 0;
}
