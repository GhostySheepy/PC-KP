#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <future>

#include "InvertedIndex.h"

using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

namespace ClientCMD
{
	void run(read_write_lock& cmd_mutex, InvertedIndex& cur_index){

		while (true) {
			write_lock lock(cmd_mutex);
			std::cout << "What word do you want to find? ('ext' for exit)" << std::endl;
			lock.unlock();
			std::string req;
			std::cin >> req;
			if (req == "ext") {
				cur_index.kill_mainloop();
				break;
			}
			std::promise<std::set<std::string>> promise;
			std::future <std::set<std::string>> future = promise.get_future();

			std::pair<std::string, std::promise<std::set<std::string>>&> formed_req = { req, promise };

			cur_index.find_request(req, promise);

			future.wait();
			std::set<std::string> res = future.get();

			lock.lock();
			std::cout << "Got results for: "<< req << std::endl;
			for (auto i : res) {
				std::cout << "\t-- " << i << std::endl;
			}
			lock.unlock();

		}
	}
};

