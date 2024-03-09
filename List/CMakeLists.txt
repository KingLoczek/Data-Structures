add_executable(lists 
    main.cpp
    arraylist.h
    arraylist.cpp
    DoublyLinkedList.h
)
target_include_directories(lists PRIVATE List/)
