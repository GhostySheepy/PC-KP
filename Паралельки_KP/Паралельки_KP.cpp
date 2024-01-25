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

#include "external/Stemming/english_stem.h"

using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

int main()
{

    std::vector<FolderIter*> vc;
    FolderIter* fi = new FolderIter("real job/test/neg_str/", 1000, 1250, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("real job/test/pos_str/", 1000, 1250, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("real job/train/neg_str/", 1000, 1250, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("real job/train/pos_str/", 1000, 1250, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("real job/train/unsup_str/", 4000, 4250, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("real job/train/unsup_str/", 4251, 4500, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("real job/train/unsup_str/", 4501, 4750, ".txt");
    vc.push_back(fi);
    fi = new FolderIter("real job/train/unsup_str/", 4751, 5000, ".txt");
    vc.push_back(fi);
    
    FolderManager folman(vc);

    Thread_Pool<std::function<void()>> tp;
    tp.initialize(4);

    auto start = std::chrono::high_resolution_clock::now();
    InvertedIndex inv_ind(folman, tp);
    
    read_write_lock cmd_mutex;

    std::thread th0(ClientCMD::run, std::ref(cmd_mutex), std::ref(inv_ind));
    //std::thread th1(ClientFake::run_fake, std::ref(cmd_mutex), std::ref(inv_ind), start);

    inv_ind.mainloop();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    th0.join();
    //th1.join();
    tp.terminate();    
}