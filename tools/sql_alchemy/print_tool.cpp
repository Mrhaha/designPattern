#include "print_tool.h"

#include <iostream>

void PrintTabSpace(int iLevel)
{
    for ( int i = 0; i < iLevel; ++i )
        std::cout << "    ";
}

void PrintEnter(int iNum /* = 1 */) {
    for ( int i = 0; i < iNum; ++i) 
        std::cout << "\n";
}

void PrintSpace(int iNum) {
    for ( int i = 0; i < iNum; ++i )
        std::cout << " ";
}

