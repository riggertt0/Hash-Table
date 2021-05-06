#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>

size_t ReadFile     (char** text, const char* file_name);
size_t GetFileSize  (FILE* file);
size_t CountSymbols (char* text, const char str);
void CreateDatabase (const char* filepath);

int main (int argc, char** argv)
{
    if (argc > 1)
        CreateDatabase (argv[1]);
    else
        CreateDatabase ("../Dicts/UTF-dict1.txt");

    return 0;
}

void CreateDatabase (const char* filepath)
{
    assert (filepath);

    char* database = nullptr;
    size_t file_size = ReadFile (&database, filepath);
    if (!file_size)
        return;

    size_t lines_num = CountSymbols (database, '\n');
    __m256i* keys         = (__m256i*) calloc (lines_num, sizeof (*keys));
    size_t*  start_points = (size_t*)  calloc (lines_num, sizeof (*start_points));

    #define check_wrong_file if (!go_file)                               \
                             {                                           \
                                 printf ("Error: Wrong file format.\n"); \
                                 free (database);                        \
                                 free (keys);                            \
                                 free (start_points);                    \
                                 return;                                 \
                             }

    char* go_file = database;

    for (size_t i_line = 0; i_line < lines_num; i_line++)
    {
        char* key = go_file;
        go_file   = strchr (go_file, '=');
        check_wrong_file;
        *go_file = '\0';
        strcpy ((char*) (keys + i_line), key);

        start_points[i_line] = (size_t) go_file - (size_t) database + 1;
        go_file = strchr (go_file + 1, '\r');

        check_wrong_file;

        go_file[0] = '\0';
        go_file[1] = '\0';
        go_file += 2;
    }

    #undef check_wrong_file

    FILE* file = nullptr;
    #define GoFile(path, data, num) if (file = fopen (path, "w"))                    \
                                    {                                                \
                                        fwrite (data, sizeof (*data), num, file);    \
                                        fclose (file);                               \
                                    }                                                \
                                    else                                             \
                                    {                                                \
                                        printf ("Warning: can't create " path "\n"); \
                                    }

    GoFile ("Database/intrinsics", keys,         lines_num);
    GoFile ("Database/pointers",   start_points, lines_num);
    GoFile ("Database/values",     database,     file_size);

    free (database);
    free (keys);
    free (start_points);

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

size_t GetFileSize (FILE* file)
{
    // ToDo: Work with files, that pointer != start
    assert (file);

    fseek (file, 0, SEEK_END);
	size_t num_symbols = ftell (file);
	fseek (file, 0, SEEK_SET);

    return num_symbols;
}

size_t CountSymbols (char* text, const char str)
{
	assert (text);
	size_t counter = 0;
	if (*text == str) 
		counter++;
	while (text = strchr (text + 1, str))
		counter++;
	return counter;
}
