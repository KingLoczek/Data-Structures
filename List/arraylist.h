#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "IList.h"

template <class T>
class ArrayList : public IList<T> {
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
        
        virtual bool contains(const T& item) override; //looks if array contains an item of type T
        int indexOf(const T& item); //index of array item
        int lastIndexOf(T item); //last index of array item
        T *get(int index); //return a type T and get whatever's in the index position
        virtual T& get(size_t index) override;
        virtual size_t size() override;

        virtual void add(T item) override; //add an item to the end of the list
        virtual void add(size_t index, T item) override; //add an item in particular index in list
        virtual void push(T item) override;
        virtual void addAll(size_t len, T* arr) override;
        void set(int index, T item); //change the spot at index to the item
        virtual T remove(size_t index) override; //remove at a particular spot
        virtual T remove() override;
        virtual T pop() override;
        // void remove(T item); //search through and find the first occurrence of an item
        
        ~ArrayList(){
            delete[] data;
        }

};

#include "arraylist.cpp"

#endif
