#include <cstdio>
#include <iostream>
#include "DoublyLinkedList.h"
#include "arraylist.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <string>

using namespace std;

int main() {
    int sel;
    std::cout << "Choose IList implementation:\n   0. ArrayList\n   3. DoublyLinkedList\n      > ";
    std::cin >> sel;

    IList<int>* list;

    switch (sel) {
        case 0:
            list = new ArrayList<int>();
            break;
        case 3:
            list = new DoublyLinkedList<int>();
            break;
        default:
            std::cout << "FATAL: Non existing IList implementation chosen " << sel << '\n';
            return 1;
    }

    string in;
    while (true) {
        std::cout << " > ";
        std::cin >> in;
        std::cout << "   ";

        if (in == "get") {
            size_t index;
            std::cin >> index;
            std::cout << list->get(index) << '\n';
        } else if (in == "add") {
            int item;
            std::cin >> item;
            list->add(item);
            std::cout << '\n';
        } else if (in == "addi") {
            size_t index;
            int item;
            std::cin >> index >> item;
            list->add(index, item);
            std::cout << '\n';
        } else if (in == "push") {
            int item;
            std::cin >> item;
            list->push(item);
            std::cout << '\n';
        } else if (in == "remove") {
            std::cout << list->remove() << '\n';
        } else if (in == "removei") {
            size_t index;
            std::cin >> index;
            std::cout << list->remove(index) << '\n';
        } else if (in == "pop") {
            std::cout << list->pop() << '\n';
        } else if (in == "view") {
            for (size_t i = 0, size = list->size(); i < size; i++) {
                std::cout << list->get(i) << ' ';
            }
            std::cout << '\n';
        } else if (in == "size") {
            std::cout << list->size() << '\n';
        } else if (in == "quit" || in == "q") {
            std::cout << "exit.\n";
            break;
        } else {
            std::cout << "ERROR: unknown command `" << in << "`\n";
        }
    }

    delete list;

    return 0;
}
