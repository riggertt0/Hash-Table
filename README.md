# Исследование хеш-таблиц

## Введение

В данной работе рассматриваются алгоритмы хеширования и способы её оптимизации хеш-таблицы. Поэтому работа разделена на 2 соответствующие части. Итак, приступим.

## 1. Алгоритмы хеширования

### 0.0) Требования к алгоритму

Хеш-таблица - структура данных, работающая с данными вида "ключ - значение" (далее - К-З). В нашем случае и ключ, и значение - строковые данные. Для быстрого доступа по ключу используется хеширование - сопоставление ключу какого-то числа, по которому производится доступ в таблицу. Т.к. хеш может получится довольно большим, сама пара ключ-значение будет храниться в ячейке ```[hash % size]```.

Вполне возможно, что значение ```hash % size``` для нескольких пар К-З совпадёт. Поэтому во время поиска по таблице придётся перебирать все ключи, соответствующие данной ячейке (они хранятся методом цепочек), и сравнивать с данным. Эта операция занимает наибольшее время, затраты на неё нужно оптимизировать.

Из этих соображений очевидно, что главное требование к алгоритму подсчёта контрольной суммы (хеша) - равномерное распределение по всем ячейкам таблицы.

### 0.1) Методика измерения

Для этой работы мы написали хеш-таблицу, в которой реализована функция заполнения из файла. На вход будет подаваться небольшой англо-русский словарь (порядка 6500 слов). Размер таблицы, будет сжат с рекомендуемых 1,5 элемента на ячейку до 10-15 для того, чтобы большинство ячеек было заполнено.

Коллизии, как было сказано ранее, будут решаться методом цепочек. Поэтому в массиве будут храниться указатели на первый элемент списка, а в самих элементах (структурах для хранения пары К-З) - на следующий. С кодом можно ознакомиться в папке "Lab 1".

Далее с помощью библиотеки matplotlib строится график, где по оси абцисс откладывается номер ячейки, а ординат - количество элементов в ячейке. По нему можно судить о равномерности распределения хеш-функции. Для лучших алгоритмов будет найдена дисперсия.

<!--- ToDo: Подсветка от Git, вставка картинок через команды md-->

### 1) Const Hash

Функция всегда возвращает одно и то же, заранее определённое значение. Это самая простая функция, время которой, к тому же, не зависит от размера ключа. Но она неэффективна, у неё вряд ли найдётся какое-либо практическое применение.

```c++
hash_t ConstHash (const char* key)
{
    return 0;
}
```

### 2) Len Hash

Функция возвращает длину ключа. Имеет узкий диапазон значений (ключи имеют длину от 2 до 30 символов) и плохое распределение. Очевидно, она не подходит для больших таблиц.

```c++
hash_t LenHash (const char* key)
{
    assert (key);
    return strlen (key);
}
```

### 3) Sum Hash

Функция возвращает сумму байт символов ключа, представленных в кодировке UTF-8 (т.к. ключ - английское слово, то эта сумма совпадает с суммой ASCII-кодов символов). Распределение плохое, т.к. числовые значения букв лежат от 97 до 122, и в значениях, кратных этим, есть очевидные пики. Возможно, при длине таблицы не более 26 этот алгоритм имел бы применение, но тогда сама таблица вряд ли является эффективной структурой для хранения данных.

```c++
hash_t SumHash (const char* key)
{
    assert (key);
    hash_t result = 0;
    while (*key != 0)
    {
        result += *key;
        key++;
    }

    return result;
}
```

### 4) SumLen Hash

Функция возвращает результат 3-го алгоритма, поделённого на результат 2-го. По своей сути является средним значением символа, поэтому имеет такое плохое распределение.

```c++
hash_t SumLenHash (const char* key)
{
    assert (key);

    hash_t len = LenHash (key);

    return (len == 0) ? 0 : SumHash (key) / len;
}
```

### 5) FirstChar

Возвращает значение первого символа. Своими проблемами очень схожа на предыдущую, хотя и имеет "более ровное" распределение.

```c++
hash_t FirstCharHash (const char* key)
{
    assert (key);
    return *key;
}
```

### 6) ROL

Эта функция производит посимвольный проход: посчитанный для предыдущих букв ключа хеш циклически сдвигается влево, после чего к нему прибавляется значение текущего символа. Функция имеет хорошее распределение, её дисперсия (среднеквадратическое отклонение) ```D = 41,1```.

```c++
hash_t RolHash (const char* key)
{
    assert (key);
    hash_t hash = 0;

    while (*key != 0)
    {
        hash = (hash << 1) + ((hash & MASK) >> ROL_ROR_SIZE) + *key; // shl (hash) + big byte + symbol
        key++;
    }

    return hash;
}
```

### 7) ROR

Алгоритм аналогичен предыдущему, только сдвиг теперь производится вправо. ```D = 28,6```.

```c++
hash_t RorHash (const char* key)
{
    assert (key);
    hash_t hash = 0;

    while (*key != 0)
    {
        hash = (hash >> 1) + ((hash & 1) << ROL_ROR_SIZE) + *key; // shl (hash) + big byte + symbol
        key++;
    }

    return hash;
}
```

### 8) CRC

Алгоритм работает как "деление в столбик", только деление заменено на побитовый XOR. В нашем случае делимым выступает ключ, а делителем - определённая константа. Более строго это объясняется как деление многочленов друг на друга, но тут мы не будет вдаваться в подробности, т.к. это долго и трудно для понимания. ```D = 17,1```.

```c++
hash_t CRC32Hash (const char* key)
{
    assert (key);
    const hash_t polynomial  = 0x04C11DB7;
    const hash_t pol_old_bit = 1 << 26;
    hash_t hash = 0x0;

    while (*key)
    {
        for (int i_bit = 7; i_bit >= 0; i_bit--)
        {
            hash = (hash << 1) + (((*key) >> i_bit) & 1); // add 1 bit
            if (hash & pol_old_bit)
                hash ^= polynomial;
        }
        key++;
    }
    return hash;
}
```

### 9) Выводы

Очевидно, что лучше всего показали себя последние 3 функции, теперь сравним их. Но тут придётся немножко заглянуть в будущее. Далее, в рамках второй части работы, они будут переписаны на языке assembly, а размер таблицы увеличен до рекомендуемого. И там результаты измерений получились следующие:

    CRC: D = 1,5  ROL: D = 2,5  ROR: D = 5,7

Таким образом, CRC превосходит циклические алгоритмы, но кто лучше: ROL или ROL - определить сложно. Наше мнение - ROL лучше, так как показал себя лучше в ситуации, более приближенной к реальности. Более того, т.к. при индексации берётся остаток от деления на размер таблицы, некоторое число старших разрядов "отбрасывается", поэтому эффективнее (в теории) должен быть алгоритм, который "чаще изменяет младшие разряды хеша, а не старшие" (извините за слишком вольное объяснение), т.е. ROL.

## 2. Ускорение работы хеш-таблицы

### 0) Общий подход к работе

Прежде всего мы реализовали функцию поиска ключа по таблице (напомню, что коллизии в ней решаются методом цепочек), после чего будем запускать поиск большого количества (100000) элементов. В нашем случае элемент для поиска будет выбираться случайно из списка: "cat", "dog", "fox", "monkey", "elephant", "chel", "fucker", "slave". Это сделано для исключения возможных случайных оптимизаций, при этом количество инструкций будет колебаться около некого среднего значения. Но это не повлияет на принимаемые решения, т.к. флуктуации составляют около 100 тысяч инструкций, а оптимизации будут делать изменения на порядок больше. Далее "бутылочное горлышко" таблицы будет устраняться и находиться новое до тех пор, пока оптимизация не станет слишком трудоёмкой, а результаты мизерными. При этом функции, используемые в main и не являющиеся членами-функциями класса HashTable, оптимизироваться не будут, так как мы считаем это пользовательским кодом, к которому мы не имеем доступ.

Программа будет собираться в 2-х конфигурациях:

Без оптимизации:

```makefile
all: main.cpp HashTable.cpp HashFunctions.cpp
    g++ main.cpp HashTable.cpp HashFunctions.cpp -Wall
    valgrind --tool=callgrind ./a.out
```

С оптимизацей ```-O3```:

```makefile
all: main.cpp HashTable.cpp HashFunctions.cpp
    g++ main.cpp HashTable.cpp HashFunctions.cpp -Wall -O3
    valgrind --tool=callgrind ./a.out
```

Данные о работе программы будет собирать утилита callgrind, после чего в KCacheGrind строится граф, в котором отражена основная информация о работе программы (функции и количество инструкций, которое выражено либо в абсолютном значении, либо в процентах от общего количества).

Стоит отметить, что мы пробовали другие средства анализа: perf и gprof. Но первый давал информацию о системных вызовах, при этом о собственных функциях программы ничего не было известно. А второй, напротив, говорил только о них, при этом значения были очень приблизительными. Утилита VTune от Intel, которая могла бы стать отличной заменой им, требовала 30 ГБ свободного места, а на рабочей установке его было всего 24 ГБ. Поэтому выбор пал на золотую середину между ними: callgrind. Стоит отметить, что, возможно, просто мы не смогли найти достаточно информации о настройке других. Если у вас есть таковая, свяжитесь, пожалуйста, с нами.

P.S. После мы узнали, что VTune не работает на процессорах от AMD, но у AMD есть своя утилита - AMD uProf. Правда, мы не стали пользоваться ею, т.к. тогда исследование подходило к концу.

### 1) Начальные характеристики

Вот результаты тестов неоптимизированной программы (см. спойлер, далее всё будет помещаться в спойлерах):

<details>
<summary> Спойлер 1, в инструкциях, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/instructions/1crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/instructions/1rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/instructions/1ror.png">
</details>

<details>
<summary> Спойлер 1, в процентах, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/percent/1crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/percent/1rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/percent/1ror.png">
</details>

<details>
<summary> Спойлер 1, в инструкциях, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/instructions/1crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/instructions/1rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/instructions/1ror.png">
</details>

<details>
<summary> Спойлер 1, в процентах, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/percent/1crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/percent/1rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/percent/1ror.png">
</details>

### 2) Улучшение 1

Первое, что бросается в глаза во время анализа программы - чрезмерно большое количество инструкций в алгоритме CRC. Без ```-O3``` количество инструкций CRC в 12 раз отличается от ROR и ROL. При этом CRC c ```-O3``` и без него тоже сильно отличаются: в 3 раза. Но на данный момент только CRC занимает много инструкций в процентном соотношении, поэтому переписать его можно позже, вместе со всеми остальными алгоритмами.

Заметим, что во время исполнения программ на алгоритмах ROL и ROR довольно большое время занимает сравнение строк (```__strcmp_avx2```): 23% и 20%, при этом сами функции занимают 6% и 7% соответственно. Это означает, что после нахождения хеша происходит долгих поиск элемента в ячейке, т.к. самих элементов в ней слишком много. Эту проблему можно решить, если уменьшить их количество, увеличив размер таблицы. Итак, изменим размер таблицы до рекомендуемых 1,5 элемента на ячейку. Мы получили такие результаты: 

<details>
<summary> Спойлер 2, в инструкциях, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/instructions/2crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/instructions/2rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/instructions/2ror.png">
</details>

<details>
<summary> Спойлер 2, в процентах, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/percent/2crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/percent/2rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/percent/2ror.png">
</details>

<details>
<summary> Спойлер 2, в инструкциях, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/instructions/2crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/instructions/2rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/instructions/2ror.png">
</details>

<details>
<summary> Спойлер 2, в процентах, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/percent/2crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/percent/2rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/percent/2ror.png">
</details>

Итог (в инструкциях, ```-O3```):

    CRC: 24,5М -> 3,4М
    ROL: 29,8М -> 7,1М
    ROR: 24,7М -> 8,7М

Очевидно, что это небольшое изменение привело к значительным улучшениям.

### 3) Улучшение 2

Стоит отметить, что CRC, благодаря лучшему распределению, меньшее количество раз обращается к ```__strcmp_avx2```. Соответственно, если переписать его, то он станет быстрее ROL и ROR.

Для этого можно использовать функции языка ассемблера: ror (циклический сдвиг вправо), rol (влево), crc. Есть несколько путей их использования:

1) Использовать ассемблерные вставки в циклах функций, остальное остаётся по-прежнему.

2) В силу специфики ключа (малый размер), его можно поместить в переменную типа ```__m256_epi8```, тогда операции сравнения ключей будут быстрее, но при этом хеш-таблица станет занимать больше места и требовать дополнительных затрат при переводе из строкового типа (если это не было сделано заранее).

3) Написать функции хеширования на assembly полностью. Это будет эффективно для сравнения, но ```strcmp``` останется тот же. Но он написан с помощью инструкций avx2.

Нами было принято решение написать именно третий вариант, т.к. выгода от второго варианта не совсем очевидна на данном этапе, но при этом его реализация потребует больших затрат. Возможно, после будет написан второй и будет возможность сравнить его с третьим. Плюсы первого перед последним же не очевидны, поэтому мы напишем последний, т.к. у него, предположительно, больше быстродействие.

Вот все функции, написанные на asm-е:

```assembly
CRC32Hash:

    push rbp 
    mov  rbp, rsp

    push rdi
    call strlen
    pop rdi
    mov rcx, rax
    xor rax, rax

    crc_while_len4:

        cmp rcx, 4
        jb crc_while_len0
        crc32 eax, dword [rdi]
        add rdi, 4
        sub rcx, 4
        jmp crc_while_len4

    crc_while_len0:

        cmp rcx, 0
        je crc_end
        crc32 eax, byte [rdi]
        inc rdi
        dec rcx
        jmp crc_while_len0

    crc_end:
    
    pop rbp
    ret
```

```assembly
RolHash:

    push rbp 
    mov  rbp, rsp
    xor rax, rax
    xor rcx, rcx

    rol_while_len0:

        mov cl, byte [rdi]
        cmp cl, 0
        je rol_end
        rol rax, 1
        add rax, rcx
        inc rdi
        jmp rol_while_len0

    rol_end:

    pop rbp
    ret
```

```assembly
RorHash:

    push rbp 
    mov  rbp, rsp
    xor rax, rax
    xor rcx, rcx

    ror_while_len0:

        mov cl, byte [rdi]
        cmp cl, 0
        je ror_end
        ror rax, 1
        add rax, rcx
        inc rdi
        jmp ror_while_len0

    ror_end:

    pop rbp
    ret
```

При этом ключи компиляции изменились на следующие:

С ```-O3```:

```makefile
all: main.cpp HashTable.cpp HashFunctions.asm
    nasm -f elf64 HashFunctions.asm
    g++ main.cpp HashTable.cpp -Wall -c -O3
    g++ main.o HashTable.o HashFunctions.o -no-pie -O3
    valgrind --tool=callgrind ./a.out
```

Без ```-O3``` - то же самое, но в строках 3 и 4 нет ```-O3``` (удивительно, не так ли?).

Функции ROL и ROR уменьшили количество инструкций с 8М до 4М, т.е. в 2 раза. А у CRC их стало 4,7М (напомню, что было 34М у -О3 и 94М без -О3), т.е. алгоритм стал значительно эффективнее благодаря инструкциям SSE4.2.

<details>
<summary> Спойлер 3, в инструкциях, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/instructions/3crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/instructions/3rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/instructions/3ror.png">
</details>

<details>
<summary> Спойлер 3, в процентах, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/percent/3crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/percent/3rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/percent/3ror.png">
</details>

<details>
<summary> Спойлер 3, в инструкциях, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/instructions/3crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/instructions/3rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/instructions/3ror.png">
</details>

<details>
<summary> Спойлер 3, в процентах, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/percent/3crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/percent/3rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/percent/3ror.png">
</details>

Кстати, тут была предпринята попытка снизить время исполнения с помощью оптимизации ```strlen```, который занимает значительную часть времени от CRC (обычный цикл на assembly), но количество инструкций оказалось больше. Вывод - попытка оказалась неудачной, изменения были удалены.

<details>
<summary> Спойлер 3.14, в инструкциях, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/instructions/3crc_blen.png">

</details>

### 4) Улучшение 3

На этом этапе было замечено, что размер таблицы не является простым числом, поэтому было принято решение исправить эту оплошность. Оптимизация оказалась успешной. Так например, для CRC без ```-O3``` количество инструкций ```__strcmp_avx2``` снизилось с 5,3М до 3,7М, т.е. на 30%.

<details>
<summary> Спойлер 4, в инструкциях, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/instructions/4crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/instructions/4rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/instructions/4ror.png">
</details>

<details>
<summary> Спойлер 4, в процентах, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/percent/4crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/percent/4rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/percent/4ror.png">
</details>

<details>
<summary> Спойлер 4, в инструкциях, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/instructions/4crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/instructions/4rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/instructions/4ror.png">
</details>

<details>
<summary> Спойлер 4, в процентах, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/percent/4crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/percent/4rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/percent/4ror.png">
</details>

### 5) Улучшение 4

Теперь пришло время попробовать вариант (см. п. 3 - улучшение 2), использующий инструкции AVX2. Для этого была изменена структура хранения пары К-З, теперь ключ хранится в вектором виде:

```c++
class Elem final: public Noncopyable
{
public:

    Elem () = default;
    Elem (const char* key, const char* value);
   ~Elem ();

    __m256i     key_   = _mm256_set1_epi8 (0);
    const char* value_;
    Elem*       next_  = nullptr;
    size_t elem_number = 0;
};
```

Все функции были переписаны под это с учётом данных особенностей. Отмечу лишь важные изменения:

1) ```__strcmp_avx2``` было заменено на команду ```_mm256_testc_si256```, которая, по-сути, возвращает 1, если векторы равны, и 0 в противном случае. Таким образом, сравнение векторов происходит очень быстро. Вот исходный код:

```c++
const char* HashTable :: Find (__m256i key)
{
    Elem* list = table_[hash_func_ (&key) % table_size_];
    while (list != nullptr)
    {
        if (_mm256_testc_si256 (key, list->key_))
            return list->value_;
        list = list->next_;
    }
    return nullptr;
}
```

2) Т.к. длина ключа стала фиксированной длины, в функции CRC не нужны проверки на конец строки, что позволяет избавиться от ```__strlen_avx2```, циклов и маленького шага подсчёта хеша:

```assembly
CRC32Hash:
    xor rax, rax
    crc32 rax, qword [rdi]
    crc32 rax, qword [rdi + 8]
    crc32 rax, qword [rdi + 16]
    crc32 rax, qword [rdi + 24]
    ret
```

Но ROR и ROL требуют побайтового исполнения, поэтому можно только конвееризовать вычисления, но всё равно это не будет так же быстро, поэтому мы не стали писать данную оптимизацию. Вот результаты тестов:

<details>
<summary> Спойлер 5, в инструкциях, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/instructions/5crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/instructions/5rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/instructions/5ror.png">
</details>

<details>
<summary> Спойлер 5, в процентах, без оптимизаций </summary>

#### __CRC__

<img src = "Lab 2/Graphics/No optimization/percent/5crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/No optimization/percent/5rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/No optimization/percent/5ror.png">
</details>

<details>
<summary> Спойлер 5, в инструкциях, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/instructions/5crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/instructions/5rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/instructions/5ror.png">
</details>

<details>
<summary> Спойлер 5, в процентах, с оптимизациями </summary>

#### __CRC__

<img src = "Lab 2/Graphics/-O3/percent/5crc.png">

#### __ROL__

<img src = "Lab 2/Graphics/-O3/percent/5rol.png">

#### __ROR__

<img src = "Lab 2/Graphics/-O3/percent/5ror.png">
</details>

Результаты впечатляют: теперь функция CRC всегда исполняется за 6 тактов. Таким образом, за 100 тысяч итераций она заняла всего лишь 600 тысяч инструкций (против 4,6М у предыдущей версии). Так же по причинам, описанным выше, не вызывается функция ```__strcmp_avx2```. Всего же с -O3 в CRC исполняется 4,2М инструкций против 11,8М, что были ранее. Правда, теперь загрузка базы данных занимает 5,5М против 2,4, что были ранее. Это происходит из-за несовершенства базы данных. Это можно будет исправить в следующей итерации.

Отмечу, что теперь измерять все показатели для ROL и ROR смысла нет, т.к. далее мы не будем изменять алгоритмы хеширования. Кстати, именно в этот  момент было принято решение измерить время исполнения для всех версий с CRC для получения наглядных качественных показателей работы таблицы.
