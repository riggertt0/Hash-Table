# Исследование хеш-функций

## Введение

В этой работе исследуется степень использования различных хэш-функций. Все измерения выполняются с помощью хеш-таблицы, основанной на методе цепочки.

В этой работе я исследую 7 различных хэш функций.  Далее я попытаюсь оптимизировать свой код, выявив наиболее долгие по времени работы функции.

Размер хеш-таблицы - 1009 (простое число), количество слов - 24000.



### 1) Const Hash

Эта функция всегда возвращает 1.
<table>
    <tr>
        <td> <img src="Data//Constant pic1.png" alt="drawing" width="400"/> </td>
    </tr>
</table>
Как видно из диаграммы, это абсолютно бесполезно.


### 2) Strlen hash

Хэш, который возвращает длину строк.
*(*(*(*)
Уже лучше, но все равно очень далеко от идеала из-за огромного числа коллизий, поэтому она бесполезна.

### 3) 	ASCII-hash

Возвращает сумму ASCII кодов символов. 
*(*(*(*)
На первый взгляд функция может показаться приемлемой, но это не так. Все так же много коллизий, примерно 1000.

### 4) First symbol hash

Хэш, который возвращает ASCII код первого символа.
*(*(*(*)
Легко видно из диаграммы, что мы сделали шаг назад по сравнению с предыдущим пунктом, эта функция ужасна и никак нам не подходит.

### 5) Xor rotate hash

Данная функция использует ROR для подсчета. Представляю код этой функции:

```c++
    uint32_t hash = 0;
    while (*string) {
        hash = hash ^ (*string);
        uint32_t oldest_bit = hash & 0x80000000;
        oldest_bit >>= 31;
        hash <<= 1;
        hash |= oldest_bit;
        ++string;
    }
```

*(*(*(*)
Это первая полезная хэш-функция поэтому интересно рассмотреть ее поподробнее. Распределение примерно равно 16-20. Такое распределение, конечно, выглядит впечатляющим на фоне предыдущих функций, но это явно не фаворит. 	

### 6) CRC32 hash	(циклический избыточный код)

Хеш-функция основана на теории циклических кодов с исправлением ошибок.
Описание процедуры: Алгоритм работает как "деление в столбик", только деление заменено на побитовый XOR. В нашем случае делимым выступает ключ, а делителем - определённая константа. Более строго это объясняется как деление многочленов друг на друга, но тут мы не будет вдаваться в подробности, т.к. это долго и трудно для понимания. 
*(*(*(*)
Из всех проанализированных функция данная является самой эффективной, посмотрим сможет ли с ней справиться Murmur2A hash.

### 7) Murmur hash

Это реализация известной хеш-функции, которая имеет меньше коллизий чем первые 5 и по своей полезности схожа с 6 пунктом.
*(*(*(*)
Он так же полезен, как и CRC32 hash.

###  Вывод: хэш-функции Murmur и CRC32 показывают лучшие результаты, это наши фавориты!!!


## 2. Ускорение работы хеш-таблицы

### 0) Общий подход к работе

Перейдем ко второй части нашей работы, оптимизации программы.

Алгоритм работы хеш-таблицы:

Чтение слов из какого-то источника.
Вычисление хеш-числа этих слов.
Этот хеш-номер является индексом в массиве нашей хеш-таблицы.
Соответствующее значение помещается в массив.
Если некоторые слова имеют одинаковый хэш (что означает один и тот же индекс в массиве), возникает коллизия. Для решения этой проблемы используются списки.
*(*(*(*)
Оптимизация Murmur выглядит не так просто, поэтому мы оптимизируем наш код CRC32.

Свою программу я буду собирать в конфигурации оптимизации O2.
Для того чтобы понять, какие функции хэш таблицы занимают наибольшее время работы, будем много раз находить перевод каждого слова в большом тексте.
Используя time-profiler, получил следующее распределение программы по времени:


### 1) Оптимизация №1

*(*(*(*)

Функция CRC32Hash :: operator () занимает порядка 93% выполнения программы! Это очень много и нужно это исправить.

Для того чтобы оптимизировать функцию проанализируем ее код.


class CRC32Hash
{
public:
    CRC32Hash() = default;
    ~CRC32Hash() = default;

    unsigned int operator()(char* string)
    {
        int length = strlen(string);
        unsigned long crc = 0xEDB88320UL;

        unsigned long crc_table[256];

        for (int i = 0; i < 256; i++)
        {
            crc = i;

            for (int j = 0; j < 8; j++)
                crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

            crc_table[i] = crc;
        };

        crc = 0xFFFFFFFFUL;

        while (length--)
            crc = crc_table[(crc ^ *string++) & 0xFF] ^ (crc >> 8);

        return (crc ^ 0xFFFFFFFFUL);
    }
};

Ошибка заключается в том что CRC-таблица пересчитывается каждый раз, именно поэтому функция выполняется так долго, это можно легко исправить.



class CRC32Hash
{
public:
    unsigned long crc = 0xEDB88320UL;
    unsigned long crc_table[256];

    CRC32Hash()
    {
        for (int i = 0; i < 256; i++)
        {
            crc = i;

            for (int j = 0; j < 8; j++)
                crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

            crc_table[i] = crc;
        };
    }

    ~CRC32Hash() = default;

    unsigned int operator()(char* string)
    {
        int length = strlen(string);

        crc = 0xFFFFFFFFUL;

        while (length--)
            crc = crc_table[(crc ^ *string++) & 0xFF] ^ (crc >> 8);

        return (crc ^ 0xFFFFFFFFUL);
    }
};

Плоды нашей работы: Среднее время работы без оптимизации составляет 2,503 секунды. Время работы с оптимизацией составляет 0,508 секунды, то есть прирост скорости составляет 393 процента.

*(*(*(*)

### 2) Оптимизация №2
Также можно использовать внутреннюю оптимизацию (встроенную инструкцию). Начиная с начального значения в crc, накапливает значение CRC32 для 32-разрядного целого числа без знака и сохраняет результат в dst. Получим:
class CRC32Hash
{
public:
    CRC32Hash() = default;
    ~CRC32Hash() = default;

    unsigned int operator()(char* string)
    {
        int length = strlen(string);
        unsigned long crc = 0xFFFFFFFFUL;
        size_t iters = length / sizeof(uint32_t);

        for (size_t i = 0; i < iters; ++i)
        {
            crc = _mm_crc32_u32(crc, *(const uint32_t*)string);
            string += sizeof(uint32_t);
        }

        return crc;
    }
};



### 3) Оптимизация №3
Первые функции здесь - это наша цель для повышения. Легко заметить, что хеш-функции принадлежит вызов strlen. Попробуем ее оптимизировать.


inline size_t fast_strlen(const char* str)
{
    unsigned int counter = 0;
    const __m256i* ptr = (__m256i*) str;
    __m256i zero = _mm256_setzero_si256();

    while (1) 
    {
        __m256i mask = _mm256_cmpeq_epi8(*ptr, zero);

        if (!_mm256_testz_si256(mask, mask)) 
            return (counter * 32) + simple_strlen((char*)ptr);

        counter++;
        ptr++;
    }
} 

Также имеет смысл соптимизировать сравнение строк. Для этого есть хорошее решение – мы можем использовать наши любимые AVX инструкции.
А теперь поподробнее. Заметим, что наши слова не превышают в размере 32-х байт. Тогда мы можем хранить ключевые слова в переменных типа __m256i. Теперь сравнение двух строк превращается в сравнение двух переменных __m256i, которое выполняется всего лишь одной инструкцией _mm256_cmpeq_epi8.

  __m256i key = _mm256_loadu_si256((const __m256i*)pair->key);
  __m256i data_key = _mm256_setzero_si256();

  for (size_t i = 0; i < list->size; ++i) {
    data_key = _mm256_loadu_si256((const __m256i*)list->data[i].key);
    int result = _mm256_movemask_epi8(_mm256_cmpeq_epi8(data_key, key));

    if (result == -1) {
      pair->value = list->data[i].value;
      return true;
    }
  }
}

Таким образом мы получили ускорение на 15%


### 8)                                        Заключение
В итоге мне удалось повысить производительность хэш таблицы на 71%.

*(*(*(*)

