#include "GenericHashTable.h"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Table *createTable(int size, int dType, int listLength)
{
    if (size <= 0 || listLength <= 0)
        return NULL;

    if (dType != STR_TYPE && dType != INT_TYPE)
        return NULL;

    Table *t = (Table *)malloc(sizeof(Table));
    if (t == NULL)
        return NULL;
    t->originalSize = size;
    t->currSize = size;
    t->type = dType;
    t->listLength = listLength;

    t->arr = (Object **)malloc(sizeof(Object *) * size);
    if (t->arr == NULL)
        return NULL;

    for (int i = 0; i < size; i++)
        t->arr[i] = NULL;

    return t;
}

void freeTable(Table *table)
{
    if (table == NULL)
        return;

    for (int i = 0; i < table->currSize; i++)
    {
        Object *curr = table->arr[i];
        Object *next = NULL;
        while (curr != NULL)
        {
            next = curr->next;
            freeObject(curr, table->type);
            curr = next;
        }
    }

    free(table->arr);
    free(table);
}

int add(Table *table, void *data)
{
    if (table == NULL || data == NULL)
        return -1;

    int place;
    if (table->type == INT_TYPE)
        place = intHashFun((int *)data, table->originalSize);
    if (table->type == STR_TYPE)
        place = strHashFun((char *)data, table->originalSize);

    // insert the data to the object
    void *dataPointer = NULL;
    if (table->type == INT_TYPE)
    {
        dataPointer = (int *)malloc(sizeof(int));
        if (dataPointer == NULL)
            return -1;
        *((int *)dataPointer) = *((int *)data);
    }
    else if (table->type == STR_TYPE)
    {
        int num = 0;
        num = numOfLetters((char *)data);
        dataPointer = (char *)malloc(sizeof(char) * num);

        if (dataPointer == NULL)
            return -1;

        strcpy(dataPointer, (char *)data);
    }

    // initialize the object
    Object *obj = createObject(dataPointer);
    if (obj == NULL)
        return -1;

    // check if there is place and if not - double the table size
    int availPlace = isAvailbePlace(table, place);
    if (availPlace == 0)
    {
        if (doubleTableSize(table) == -1)
            return -1;
    }

    // find how many enteris there is for each hash value
    int blockSize = table->currSize / table->originalSize;
    int to = blockSize * (place + 1);

    for (int i = place * blockSize; i < to; i++)
    {
        Object *temp = table->arr[i];
        if (temp == NULL)
        {
            table->arr[i] = obj;
            return i;
        }

        int j = 0;
        while (temp->next != NULL && j < table->listLength)
        {
            j++;
            temp = temp->next;
        }

        if (j < table->listLength - 1)
        {
            temp->next = obj;
            return i;
        }
    }

    return -1;
}

int removeObj(Table *table, void *data)
{
    if (table == NULL || data == NULL)
        return -1;

    // find the starting search point
    int place;
    if (table->type == INT_TYPE)
        place = intHashFun((int *)data, table->originalSize);

    if (table->type == STR_TYPE)
        place = strHashFun((char *)data, table->originalSize);

    // find how many enteris there is for each hash value
    int blockSize = table->currSize / table->originalSize;
    int to = blockSize * (place + 1);

    // search between the relevant places to get o(1)
    for (int i = place * blockSize; i < to; i++)
    {
        Object *prev = NULL;
        Object *curr = table->arr[i];
        while (curr != NULL)
        {
            if (isEqual(table->type, curr->data, data) == 0)
            {
                if (prev != NULL)
                    prev->next = curr->next;

                else if (prev == NULL)
                    table->arr[i] = curr->next;

                freeObject(curr, table->type);
                return i;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    return -1;
}

Object *search(Table *table, void *data)
{
    if (table == NULL || data == NULL)
        return NULL;

    // find the starting search point
    int place;
    if (table->type == INT_TYPE)
        place = intHashFun((int *)data, table->originalSize);

    if (table->type == STR_TYPE)
        place = strHashFun((char *)data, table->originalSize);

    // find how many enteris there is for each hash value
    int blockSize = table->currSize / table->originalSize;
    int to = blockSize * (place + 1);

    // search between the relevant places to get o(1)
    for (int i = place * blockSize; i < to; i++)
    {
        Object *obj = table->arr[i];
        while (obj != NULL)
        {
            if (isEqual(table->type, obj->data, data) == 0)
                return obj;
            obj = obj->next;
        }
    }

    return NULL;
}

void printTable(Table *table)
{
    if (table == NULL)
        return;

    for (int i = 0; i < table->currSize; i++)
    {
        printf("[%d]\t", i);

        Object *obj = table->arr[i];
        if (table->type == INT_TYPE)
        {
            while (obj != NULL)
            {
                printf("%d\t-->\t", *(int *)obj->data);
                obj = obj->next;
            }
        }

        if (table->type == STR_TYPE)
        {
            while (obj != NULL)
            {
                printf("%s\t-->\t", (char *)obj->data);
                obj = obj->next;
            }
        }

        printf("\n");
    }
}

Object *createObject(void *data)
{
    if (data == NULL)
        return NULL;

    Object *obj = (Object *)malloc(sizeof(Object));
    if (obj == NULL)
        return NULL;

    obj->data = data;
    obj->next = NULL;
    return obj;
}

void freeObject(Object *obj, int type)
{
    if (obj == NULL)
        return;

    free(obj->data);
    free(obj);
}

int isEqual(int type, void *data1, void *data2)
{
    if (data1 == NULL || data2 == NULL)
        return -1;

    // int type
    if (type == INT_TYPE)
    {
        if (*(int *)data1 == *(int *)data2)
            return 0;
        return -1;
    }

    // string type
    if (type == STR_TYPE)
    {
        char *str1 = (char *)data1;
        char *str2 = (char *)data2;

        if (strcmp(str1, str2) == 0)
            return 0;
        return -1;
    }

    return -1;
}

int intHashFun(int *key, int origSize)
{
    if (key == NULL || origSize <= 0)
        return -1;

    int res = *key % origSize;
    if (res < 0)
        res = res + origSize;

    return res;
}

int strHashFun(char *key, int origSize)
{
    if (key == NULL || origSize <= 0)
        return -1;

    int sum = 0;
    int i = 0;
    while (key[i] != '\0')
    {
        sum += (int)key[i];
        i++;
    }

    return sum % origSize;
}

// personal functions
int numOfLetters(char *text)
{
    int num = 1;
    int i = 0;

    while (text[i] != '\0')
    {
        num++;
        i++;
    }

    return num;
}

int isAvailbePlace(Table *table, int place)
{
    int blockSize = table->currSize / table->originalSize;
    int to = blockSize * (place + 1);

    for (int i = place * blockSize; i < to; i++)
    {
        Object *temp = table->arr[i];
        if (temp == NULL)
        {
            return 1;
        }

        int j = 0;
        while (temp != NULL)
        {
            j++;
            temp = temp->next;
        }
        if (j < table->listLength)
        {
            return 1;
        }
    }
    return 0;
}

int doubleTableSize(Table *table)
{
    int oldSize = table->currSize;
    int newSize = oldSize * 2;

    Object **temp = (Object **)malloc(sizeof(Object *) * oldSize);
    if (temp == NULL)
        return -1;

    for (int i = 0; i < oldSize; i++)
        temp[i] = NULL;

    for (int i = 0; i < oldSize; i++)
        temp[i] = table->arr[i];

    table->arr = (Object **)realloc(table->arr, sizeof(Object *) * newSize);
    if (table->arr == NULL)
        return -1;
    table->currSize = newSize;

    // initialize memory
    for (int i = 0; i < newSize; i++)
        table->arr[i] = NULL;

    // insert values
    for (int i = 0; i < oldSize; i++)
        table->arr[i * 2] = temp[i];

    free(temp);
    return 1;
}