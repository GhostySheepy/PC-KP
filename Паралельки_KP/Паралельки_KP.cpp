// Паралельки_KP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <locale>
#include <codecvt>
#include <string>

#include "InvertedIndex.h"

#include "ClientCMD.h"
#include "ClientFake.h"
//#include <chrono>


using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

int main()
{
    std::vector<FolderIter*> vc;
    FolderIter* fi = new FolderIter("test/", 0, 1, ".txt");
    vc.push_back(fi);
    /*fi = new FolderIter("C:/NEW1/", 0, 2, ".txt");
    vc.push_back(fi);*/
    
    FolderManager folman(vc);

    Thread_Pool<std::function<void()>> tp;
    tp.initialize(2);

    auto start = std::chrono::high_resolution_clock::now();
    InvertedIndex inv_ind(folman, tp);
    
    read_write_lock cmd_mutex;
    //std::thread th(ClientCMD::run, std::ref(cmd_mutex), std::ref(inv_ind));
    std::thread th(ClientFake::run_fake, std::ref(cmd_mutex), std::ref(inv_ind), start);

    inv_ind.mainloop();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    th.join();
    tp.terminate();    
}