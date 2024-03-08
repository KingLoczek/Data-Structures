#ifndef ARRAYLIST_H
#include "arraylist.h"
#endif

template <class T>
void ArrayList<T>::resize()
{
    // Resize the array to accomodate more data
    // New temp array
    T *temp = new T[arrLenght * 2];
    for(int i=0; i<arrLenght; i++)
        temp[i]=data[i];
    delete[] data; // Free the memory allocated to the old array
    data = temp;
    arrLenght *= 2;
}

template <class T>
bool ArrayList<T>::needToResize()
{
    // Return if the data is equal to the array lenght
    return arrLenght == listSize;
}

template <class T>
void ArrayList<T>::add(T item)
{
    // Add the item to the end of the list

    // Check to see if there's enough space
    if(needToResize()) resize();

    data[listSize] = item;
    listSize ++;
}

template <class T>
void ArrayList<T>::add(int index, T item)
{
    // Add the item at the specified index,
    // shifting other elements down

    if(needToResize()) resize();
    // Shift elements to the right to make space for the new item
    for (int i = 0; i < listSize; i--)
        data[listSize+1] = data[listSize];

    data[index] = item;
    listSize ++;
}

template <class T>
void ArrayList<T>::remove(int index)
{   
    // Remove the item at the specified index

    if(index >= 0 && index <= listSize)
    {   
        // Shift elements to the left to fill the gap
        for (int i = index; i < listSize; i++)
            data[i] = data[i+1];
    }
    listSize --;   
}

template <class T>
T* ArrayList<T>::get(int index)
{
    // Return a pointer to the item at the specified index
    if(index >= 0 && index <= listSize)
        return &data[index];
    return nullptr;
}

template <class T>
void ArrayList<T>::set(int index, T item)
{
    // Change the item at the specified index to the new item
    if (index >= 0 && index <= listSize)
        data[index] = item;
}

template <class T>
int ArrayList<T>::indexOf(T item)
{
    // Find the index of the first occurrence of the item
    for (int i = 0; i < listSize; i++)
    {
        if (item == data[i])
            return i;
    }
    return -1; // Return -1 if the item is not found
}

template <class T>
int ArrayList<T>::lastIndexOf(T item)
{
    // Find the index of the last occurrence of the item
    for (int i = listSize; i >= 0; i--)
    {
        if (item == data[i])
            return i;
    }
    return -1; // Return -1 if the item is not found
}

template <class T>
bool ArrayList<T>::contains(T item)
{
    // Check if the list contains the specified item
    return(indexOf(item) > -1);
}

template <class T>
int ArrayList<T>::size()
{
    // Return the number of elements in the list
    return listSize;
}