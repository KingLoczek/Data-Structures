#ifndef ILIST_H
#define ILIST_H

#include <cstddef>

template<typename T>
class IList {
    public:
        /**
         * Destroys the list
         * @implNote Virtual destructor is needed for object descrution using IList pointer
         */
        virtual ~IList() {}

        /**
         * Get element at index _index_
         * Fails if index is out of bounds
         */
        virtual T& get(size_t index) = 0;

        /**
         * Adds an element to the end of the list
         */
        virtual void add(T element) = 0;

        /**
         * Adds an element to the begining of the list
         */
        virtual void push(T element) = 0;

        /**
         * Adds an element at the given index
         * Falis if index is out of bounds
         */
        virtual void add(size_t index, T element) = 0;

        /**
         * Removes an element from the end of the list
         * Fails if the list is empty
         */
        virtual T remove() = 0;

        /**
         * Removes an element from the beginning of the listprepend
         * Fails if the list is empty
         */
        virtual T pop() = 0;

        /**
         * Removes an element at the given index
         * Fails if index is out of bounds
         */
        virtual T remove(size_t index) = 0;

        /**
         * Checks if an element is present in the list
         */
        virtual bool contains(const T& element) = 0;


        /**
         * Returns the size of the list
         */
        virtual size_t size() = 0;

};

#endif
