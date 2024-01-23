// Паралельки_KP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <locale>
#include <codecvt>
#include <string>

#include "external\Stemming\english_stem.h"

#include "external\boost\algorithm\string.hpp"
#include "external\boost\algorithm\string\classification.hpp"
#include "external\boost\algorithm\string\split.hpp"


#include "FolderManager.h"
#include "InvertedIndex.h"
#include "ThreadPool.h"


int main()
{
    std::cout << "hell0" << std::endl;


    std::vector<FolderIter*> vc;

    /*FolderIter* fi = new FolderIter("C:/NEW/", 0, 2, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("C:/NEW1/", 0, 2, ".txt");
    vc.push_back(fi);*/
    

    FolderManager folman(vc);
    Thread_Pool<std::function<void()>> tp;
    tp.initialize(2);

    InvertedIndex inv_ind(folman, tp);
    std::cout << "hell0" << std::endl;
    tp.terminate();
    std::cout << "hell0" << std::endl;
    /*std::vector<std::string> res = folman.get_filenames(5);

    int i = 0;
    for (std::string n : res) {
        std::cout << i << ") " << n << ' ' << std::endl;
        i++;
    }

    std::cout << folman.is_empty() << std::endl;
        


    res = folman.get_filenames(5);
    i = 0;
    for (std::string n : res) {
        std::cout << i << ") " << n << ' ' << std::endl;
        i++;
    }
    std::cout << folman.is_empty() << std::endl;

    res = folman.get_filenames(5);
    i = 0;
    for (std::string n : res) {
        std::cout << i << ") " << n << ' ' << std::endl;
        i++;
    }
    std::cout << folman.is_empty() << std::endl;*/
    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
