#include <iostream>
#include "arraylist.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(){

    ArrayList<int> myList;

    // Adding elements to the list
    myList.add(5);
    myList.add(10);
    myList.add(15);
    myList.add(0,2);

    // Printing the size of the list
    cout << "Size of the list: " << myList.size() << endl;

    // Checking if the list contains a certain element
    cout << "Does the list contain 10? " << (myList.contains(10) ? "Yes" : "No") << endl;

    // Adding elements to the list
    for (int i = 0; i < 20; ++i) {
        myList.add(i * 10);
        std::cout << "Size of the list after adding element " << i+1 << ": " << myList.size() << std::endl;
    }

    // Printing the elements of the list
    std::cout << "Elements of the list:" << std::endl;
    for (int i = 0; i < myList.size(); i++) {
        std::cout << *(myList.get(i)) << " ";
    }
    std::cout << std::endl;

    // Removing the element at index 2
    myList.remove(2);

    // Printing the updated size of the list
    std::cout << "Size of the list after removal: " << myList.size() << std::endl;

    return 0;
}