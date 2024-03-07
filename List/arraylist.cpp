#ifndef ARRAYLIST_H
#include "arraylist.h"
#endif

template <class T>
void ArrayList<T>::resize()
{
    //resize the array to accomodate more data
    

    //new temp array
    T *temp = new T[arrLenght * 2];
    for(int i=0; i<arrLenght; i++)
        temp[i]=data[i];
    data = temp;
    arrLenght *= 2;
}

template <class T>
bool ArrayList<T>::needToResize()
{
    //return if the data is equal to the array lenght
    return arrLenght == listSize;
}

template <class T>
void ArrayList<T>::add(T item)
{
    //add the item to the end of the list

    //check to see if there's enough space
    if(needToResize()) resize();

    data[listSize] = item;
    listSize ++;
}

template <class T>
void ArrayList<T>::add(int index, T item)
{
    //add the item to the indicated index,
    //everything else gets bumped down

    if(needToResize()) resize();

    //
    for (int i = 0; i < listSize; i--)
        data[listSize+1] = data[listSize];

    data[index] = item;
    listSize ++;
    
}