#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "HashFunctions.h"

struct Noncopyable
{
    Noncopyable () {}

    Noncopyable (const Noncopyable& elem) = delete;
    Noncopyable& operator = (const Noncopyable& elem) = delete; 

    Noncopyable (const Noncopyable&& elem) = delete;
    Noncopyable& operator = (const Noncopyable&& elem) = delete; 
};


struct Elem: Noncopyable
{
    const char* key_   = nullptr;
    const char* value_ = nullptr;
    Elem*       next_  = nullptr;
    size_t elem_number = 0;
    // ToDo: Good idea: 2 types of Elem: allocated and not-allocated

    Elem ();
    Elem (const char* key, const char* value);
   ~Elem ();
};


class HashTable: Noncopyable
{
    Elem** table_   = nullptr;
    char* database_ = nullptr;
    size_t table_size_ = 0;

    void AddKey (const char* key, const char* value);
    hash_t (*hash_func_) (const char*) = nullptr;

    public:

    HashTable (size_t size, hash_t (*hash_func) (const char*));
   ~HashTable(); 

    //! @note: File format: strings key=value, EOL = \r\n
    void FillTable (const char* filename);
    void GetStatistic ();
    const char* Find (const char* key);
};

size_t ReadFile (char** text, const char* file_name);
size_t GetFileSize (FILE* file);
