#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <immintrin.h>
#include "HashFunctions.h"

const size_t KEY_SIZE = 31;

class Noncopyable // Можно и макрос, но хз.
{
public:

    Noncopyable () = default;

    Noncopyable (const Noncopyable& elem) = delete;
    Noncopyable& operator = (const Noncopyable& elem) = delete; 

    Noncopyable (const Noncopyable&& elem) = delete;
    Noncopyable& operator = (const Noncopyable&& elem) = delete; 
};


class Elem final: public Noncopyable
{
public:

    Elem () = default;
    Elem (__m256i* key, const char* value);
   ~Elem ();

    __m256i*    key_;
    const char* value_;
    Elem*       next_  = nullptr;
    size_t elem_number = 0;
};


class HashTable final: public Noncopyable
{
public:

    HashTable (size_t size, hash_t (*hash_func) (__m256i*));
   ~HashTable(); 

    //! @note: File format: strings key=value, EOL = \r\n
    void FillTable ();
    void GetStatistic ();
    const char* Find (__m256i key);

private:

    __m256i*     keys_ = nullptr;
    Elem**      table_ = nullptr;
    char*      values_ = nullptr;
    Elem*   elem_data_ = nullptr;
    size_t table_size_ = 0;

    inline void AddKey (Elem* elem, __m256i* key, const char* value);
    hash_t (*hash_func_) (__m256i*) = nullptr;
};

size_t ReadFile (char** text, const char* file_name);
size_t GetFileSize (FILE* file);
