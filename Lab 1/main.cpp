#include "HashTable.h"

int main()
{
    HashTable table (400, CRC32Hash);
    // table.table_[0] = new Elem {"123", 3, "456", 4};
    table.FillTable ("../Dicts/UTF-dict1.txt");
    table.GetStatistic ();
}
