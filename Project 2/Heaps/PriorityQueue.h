#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

struct Node
{
    int data;
    int priority;
};

template<typename T>
class PriorityQueue
{
public:
    /**
    * Addition of element with priority 
    */
    virtual void insert(T element) = 0;

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
    virtual void modifyKey(int data, int newPriority) = 0;

    /**
    * Returns the size of the list
    */
    virtual int returnSize() = 0;
};

#endif