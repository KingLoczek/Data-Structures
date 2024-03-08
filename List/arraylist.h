#ifndef ARRAYLIST_H
#define ARRAYLIST_H

template <class T>
class ArrayList{
    private:
        T *data;
        int arrLenght; //lenght of the actual array
        int listSize; //number of elements
        void resize(); //resize the array if necessary
        bool needToResize(); //determine if we need to resize
    public:
        ArrayList(){
            //get an array set up
            data = new T[5];
            //initialize parameters
            arrLenght = 5;
            listSize = 0;
        }
        
        bool contains(T item); //looks if array contains an item of type T
        int indexOf(T item); //index of array item
        int lastIndexOf(T item); //last index of array item
        T *get(int index); //return a type T and get whatever's in the index position
        int size();

        void add(T item); //add an item to the end of the list
        void add(int index, T item); //add an item in particular index in list
        void set(int index, T item); //change the spot at index to the item
        void remove(int index); //remove at a particular spot
        // void remove(T item); //search through and find the first occurrence of an item
        
        ~ArrayList(){
            delete[] data;
        }

};

#include "arraylist.cpp"

#endif