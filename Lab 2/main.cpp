#include "HashTable.h"
#include <stdlib.h>
#include <time.h>

extern const size_t KEY_SIZE; // ToDo: Part of class

int main()
{
    srand (time (0));
    HashTable table (4409, CRC32Hash);
    table.FillTable ("../Dicts/UTF-dict1.txt");
    // table.GetStatistic ();

    const char* test_list_[] = {"cat", "dog", "fox", "monkey", "elephant", "chel", "fucker", "slave"};
    __m256i test_list[8] = {};

    for (size_t i_test = 0; i_test < 8; i_test++)
        strncpy ((char*) (test_list + i_test), test_list_[i_test], KEY_SIZE);

    for (int i_iter = 0; i_iter < 100000; i_iter++)
    {
        const char* res = table.Find (test_list[rand() % (sizeof (test_list) / sizeof (*test_list))]);
        if (res) 
            printf ("%s\n", res);
        else
            printf ("Извините, я не знаю такое слово.\n");
    }
}
