#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "IList.h"

template <class T>
class ArrayList : public IList<T> {
    private:
        T *data;
        int arrLenght; //lenght of the actual array
        int listSize; //number of elements 
        void resize()//resize the array if necessary
        {
            // Resize the array to accomodate more data
            // New temp array
            T *temp = new T[arrLenght * 2];
            for(size_t i=0; i<listSize; i++) // list only contains listSize elements to copy over
                temp[i]=data[i];
            delete[] data; // Free the memory allocated to the old array
            data = temp;
            arrLenght *= 2;
        };
        bool needToResize() //determine if we need to resize
        {
            // Return if the data is equal to the array lenght
            return arrLenght == listSize;
        };   
    public:
        ArrayList(){
            //get an array set up
            data = new T[5];
            //initialize parameters
            arrLenght = 5;
            listSize = 0;
        };
        
        virtual bool contains(const T& item) override //looks if array contains an item of type T
        {
            // Check if the list contains the specified item
            return(indexOf(item) > -1);
        };

        int indexOf(const T& item) //index of array item
        {
            // Find the index of the first occurrence of the item
            for (int i = 0; i < listSize; i++)
            {
                if (item == data[i])
                    return i;
            }
            return -1; // Return -1 if the item is not found
        };

        int lastIndexOf(T item) //last index of array item
        {
            // Find the index of the last occurrence of the item
            for (int i = listSize; i >= 0; i--)
            {
                if (item == data[i])
                    return i;
            }
            return -1; // Return -1 if the item is not found
        };

        T *get(int index) //return a type T and get whatever's in the index position
        {
            // Return a pointer to the item at the specified index
            if(index >= 0 && index <= listSize)
                return &data[index];
            return nullptr;
        };

        virtual T& get(size_t index) override
        {
            assert(index < listSize && "Out of bounds");
            return data[index];
        };
        
        virtual size_t size() override
        {
            // Return the number of elements in the list
            return listSize;
        };

        virtual void add(T item) override //add an item to the end of the list
        {
            // Add the item to the end of the list
            // Check to see if there's enough space
            if(needToResize()) resize();
            data[listSize] = item;
            listSize ++;
        };
        
        virtual void add(size_t index, T item) override //add an item in particular index in list
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
        };

        virtual void push(T item) override
        {
            if(needToResize()) resize();

            for (size_t i = listSize; i > 0; i--)
                data[i] = data[i - 1];
            data[0] = item;
            listSize++;
        };
        
        virtual void addAll(size_t len, T* arr) override
        {
            for (size_t i = len - 1; i > 0; i--) {
                add(std::move(arr[i]));
            }
            if (len > 0) add(std::move(arr[0]));
        };

        void set(int index, T item) //change the spot at index to the item
        {
            // Change the item at the specified index to the new item
            if (index >= 0 && index <= listSize)
                data[index] = item;
        };

        virtual T remove(size_t index) override //remove at a particular spot
        {   
            assert(index < listSize && "Out of bounds");
            // Remove the item at the specified index

            T removed(std::move(data[index]));
            // Shift elements to the left to fill the gap
            for (size_t i = index; i < listSize; i++)
                data[i] = data[i+1];
            listSize --;   
            return removed;
        };

        virtual T remove() override
        {
            return remove(listSize - 1);
        };

        virtual T pop() override // void remove(T item); //search through and find the first occurrence of an item
        {
            return remove(0);
        };
        
        ~ArrayList(){
            delete[] data;
        };

};


#endif
