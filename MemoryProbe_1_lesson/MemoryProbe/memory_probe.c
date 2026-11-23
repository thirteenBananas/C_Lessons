#include <stdio.h>
#include <stdlib.h>

void probe_dangling(void);
void probe_double_free(void);

int main(void)
{
    
    int a = 100;
    int *p = &a; // создал указатель на переменную a
    
    int *heap_ptr = malloc(sizeof(int)); // создал указатель и выделил память для одного инта
    
    if(heap_ptr == NULL)
    {
        printf("Ошибка! malloc() вернул NULL");
        return EXIT_FAILURE; // завершение ошибкой
    }
    

    *heap_ptr = 500;
    printf("Адрес выделенной памяти: %p | Записанное значение: %d \n", (void *)heap_ptr, *heap_ptr);
    
    free(heap_ptr); // освободил память
    
    printf("Значение переменной: %d | Адрес переменной: %p \n", a, (void *)&a);
    printf("Значение указателя: %p | Результат его разыменовывания: %d \n", (void *)p, *p); // явное приведение
    
    *p = 200; // поменял значение переменной a через указатель
    printf("Новое значение переменной: %d \n", a);
    
    printf("Размер int: %zu байт| Размер указателя:  %zu байт \n", sizeof(int), sizeof(p));
    //probe_dangling();
    probe_double_free();
    
    return 0;
    
    /* До вызова free(heap_ptr) у нас есть действительный объект со значением 500.
     
     После free(heap_ptr):
     - Время жизни выделенного объекта заканчивается.
     - Переменная heap_ptr продолжает существовать.
     - free() не записывает автоматически NULL в heap_ptr.
     - Разыменовывать старый указатель больше нельзя — это UB.
     Техническая тонкость: по стандарту C значение указателя на объект после окончания времени жизни этого объекта становится неопределённым. Поэтому даже если LLDB продолжает показывать прежние адресные биты, использовать этот адрес как действительный указатель нельзя.
     
     В качестве защитной практики после освобождения единственного указателя-владельца часто пишут:
     free(heap_ptr);
     heap_ptr = NULL;
     
     Но это не исправляет другие указатели, если они тоже ссылались на тот же блок. */
}


void probe_dangling(void)
{
    int *p = malloc(sizeof(int));
    
    if(p == NULL)
    {
        printf("Ошибка! malloc() вернул NULL\n");
        return;
    }
    
    *p = 777;
    
    int *alias = p;
    printf("Значение p = %d\n", *alias);
    
    free(p);
    
    p = NULL;
    printf("Число: %d\n", *alias); // heap-use-after-free
}

void probe_double_free(void)
{
    int *a = malloc(sizeof(int));
    if(a == NULL)
    {
        printf("Ошибка! malloc() вернул NULL\n");
        return;
    }
    
    *a = 500;
    
    free(a);
    a = NULL;
    free(a); // undefined behavior,ERROR: AddressSanitizer: attempting double-free
}

void probe_leak(void)
{
    int *array = malloc(10 * sizeof(int));;
    
    for (int i = 0; i < 10 ; ++i)
    {
        array[i] = i + 1;
        printf("%d", array[i]);
    }
    array = NULL;
    
}
