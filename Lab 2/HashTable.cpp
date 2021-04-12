#include "HashTable.h"

Elem :: Elem () {return;}

/*
Elem :: Elem (const char* key, const char* value):
    key_   (strdup (key)),
    value_ (strdup (value))
{
    assert (key_);
    assert (value_);

    // ToDo: throw exception
    // ToDo: Fixed size (64), SSE type

    return;
} */

Elem :: Elem (const char* key, const char* value):
    key_   (key),
    value_ (value)
{
    assert (key_);
    assert (value_);
}

Elem :: ~Elem()
{
    delete next_;
    // delete key_;
    // delete value_;
    next_  = nullptr;
    key_   = nullptr;
    value_ = nullptr;

    return;
}

HashTable :: HashTable (size_t size, hash_t (*hash_func) (const char*)):
    table_      (new Elem* [size]),
    table_size_ (size),
    hash_func_  (hash_func)
{
    return;
}

HashTable :: ~HashTable ()
{
    for (size_t i_elem = 0; i_elem < table_size_; i_elem++)
        delete table_[i_elem];
    table_size_ = 0;
    delete table_;
    delete database_;
}

void HashTable :: FillTable (const char* filename)
{
    assert (filename);
    if (database_ != nullptr)
    {
        printf ("Error: Table is already created.\n");
        return;
    }

    ReadFile (&database_, filename);
    if (!database_)
        return;
    
    char* database = database_;
    
    #define check_wrong_file if (!database)                              \
                             {                                           \
                                 printf ("Error: Wrong file format.\n"); \
                                 delete database_;                       \
                                 return;                                 \
                             }

    while (*database)
    {
        char* key = database;
        database = strchr (database, '=');

        check_wrong_file;

        *database = '\0';
        char* value = database + 1;
        database = strchr (database + 1, '\r');

        check_wrong_file;

        database[0] = '\0';
        database[1] = '\0';
        database += 2;
        AddKey (key, value);
    }

    #undef check_wrong_file
}

void HashTable :: AddKey (const char* key, const char* value)
{
    assert (key);
    assert (value);

    Elem* elem        = new Elem (key, value);
    hash_t position   = hash_func_ (key) % table_size_;
    elem->elem_number = table_[position] ? table_[position]->elem_number + 1 : 1;
    elem->next_       = table_[position];
    table_[position]  = elem;
}

void HashTable :: GetStatistic ()
{
    if (!table_ || !database_)
    {
        printf ("Can't get statistic: table wasn't created or initialised.\n");
        return;
    }

    printf ("Get statistic: Creating data...\n");

    FILE* stat = fopen ("Statistic.csv", "w");

    for (size_t i_elem = 0; i_elem < table_size_; i_elem++)
        fprintf (stat, "%lu,", table_[i_elem] ? table_[i_elem]->elem_number : 0);

    fseek (stat, -1, SEEK_CUR);
    fprintf (stat, "\n");
    fclose (stat);

    printf ("Get statistic: Creating graphic...\n");
    system ("python3 Graphic.py");
    printf ("Get statistic: Complete!\n");

    return;
}

size_t ReadFile (char** text, const char* file_name)
{
    assert (text);
	assert (file_name);

	FILE* file = fopen (file_name, "rb");
	if (!file)
	{
		printf ("[Input error] Unable to open file \"%s\"\n", file_name);
		return 0;
	}

    size_t num_symbols = GetFileSize (file);

	*text = new char[(num_symbols + 1) * sizeof(**text)];

	if (!text)
	{
		printf ("[Error] Unable to allocate memory\n");
        fclose (file);
		return 0;
	}

	if (fread (*text, sizeof(**text), num_symbols, file) != num_symbols)
    {
	    fclose (file);
        return 0;
    }

    fclose (file);
	return num_symbols;
}

// ToDo: Work with files, that pointer != start
size_t GetFileSize (FILE* file)
{
    assert (file);

    fseek (file, 0, SEEK_END);
	size_t num_symbols = ftell (file);
	fseek (file, 0, SEEK_SET);

    return num_symbols;
}

const char* HashTable :: Find (const char* key)
{
    assert (key);

    Elem* list = table_[hash_func_ (key) % table_size_];

    while (list != nullptr)
    {
        if (strcmp (key, list->key_) == 0)
            return list->value_;
        
        list = list->next_;
    }

    return nullptr;
}
