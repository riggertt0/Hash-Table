#include "HashTable.h"
#include <stdlib.h>
#include <time.h>

int main()
{
    srand (time (0));
    const char* test_list[] = {"cat", "dog", "fox", "monkey", "elephant", "chel", "fucker", "slave"};
    HashTable table (400, CRC32Hash);
    table.FillTable ("../Dicts/UTF-dict1.txt");

    for (int i_iter = 0; i_iter < 100000; i_iter++)
    {
        const char* res = table.Find (test_list[rand() % (sizeof (test_list) / sizeof (*test_list))]);
        if (res) 
            printf ("%s\n", res);
        else
            printf ("Извините, я не знаю такое слово.\n");
    }
}
