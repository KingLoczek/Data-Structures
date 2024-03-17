#ifndef ARRAYLIST_H
#include "arraylist.h"
#endif

#include <memory>
#include <cassert>

template <class T>
void ArrayList<T>::resize()
{
    // Resize the array to accomodate more data
    // New temp array
    T *temp = new T[arrLenght * 2];
    for(size_t i=0; i<listSize; i++) // list only contains listSize elements to copy over
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
void ArrayList<T>::add(size_t index, T item)
{
    assert(index < listSize && "Out of bounds!");
    // Add the item at the specified index,
    // shifting other elements down

    if(needToResize()) resize();
    // Shift elements to the right to make space for the new item
    for (size_t i = listSize - 1; i > index; i--)
        data[i + 1] = data[i];
    data[index + 1] = data[index];

    data[index] = item;
    listSize ++;
}

template<class T>
void ArrayList<T>::addAll(size_t len, T* arr) {
    for (size_t i = len - 1; i > 0; i--) {
        add(std::move(arr[i]));
    }
    if (len > 0) add(std::move(arr[0]));
}

template<class T>
void ArrayList<T>::push(T item) {
    if(needToResize()) resize();

    for (size_t i = listSize; i > 0; i--)
        data[i] = data[i - 1];
    data[0] = item;
    listSize++;
}

template <class T>
T ArrayList<T>::remove(size_t index)
{   
    assert(index < listSize && "Out of bounds");
    // Remove the item at the specified index

    T removed(std::move(data[index]));
    // Shift elements to the left to fill the gap
    for (size_t i = index; i < listSize; i++)
        data[i] = data[i+1];
    listSize --;   
    return removed;
}

template<class T>
T ArrayList<T>::remove() {
    return remove(listSize - 1);
}

template<class T>
T ArrayList<T>::pop() {
    return remove(0);
}

template <class T>
T* ArrayList<T>::get(int index)
{
    // Return a pointer to the item at the specified index
    if(index >= 0 && index <= listSize)
        return &data[index];
    return nullptr;
}

template<class T>
T& ArrayList<T>::get(size_t index) {
    assert(index < listSize && "Out of bounds");
    return data[index];
}

template <class T>
void ArrayList<T>::set(int index, T item)
{
    // Change the item at the specified index to the new item
    if (index >= 0 && index <= listSize)
        data[index] = item;
}

template <class T>
int ArrayList<T>::indexOf(const T& item)
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
bool ArrayList<T>::contains(const T& item)
{
    // Check if the list contains the specified item
    return(indexOf(item) > -1);
}

template <class T>
size_t ArrayList<T>::size()
{
    // Return the number of elements in the list
    return listSize;
}
