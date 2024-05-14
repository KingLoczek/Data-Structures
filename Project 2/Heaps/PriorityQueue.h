#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <cstddef>

template<typename T, typename P>
class PriorityQueue
{
public:
    /**
    * Addition of element with priority 
    */
    virtual void insert(P priority, T element) = 0;

    /**
    * Remove and return the element with the highest priority
    */
    virtual T extractMax() = 0;

    /**
    * Return the element with the highest priority
    */
    virtual T findMax() = 0;

    /**
    * Change the priority of the element. 
    */
    virtual void modifyKey(T data, P newPriority) = 0;

    /**
    * Returns the size of the list
    */
    virtual size_t size() = 0;
};

#endif
