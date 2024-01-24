#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <chrono>

#include "InvertedIndex.h"

using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

namespace ClientFake
{
	void run_fake(read_write_lock& cmd_mutex, InvertedIndex& cur_index, std::chrono::time_point<std::chrono::high_resolution_clock> start) {
		
		int x = 0;
		while (x < 1) {
			std::promise<std::set<std::string>> promise;
			std::future<std::set<std::string>> future = promise.get_future();

			cur_index.find_request("like", promise);

			future.wait();
			std::set<std::string> res = future.get();

			write_lock lock(cmd_mutex);
			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration<double, std::chrono::seconds::period>(end - start).count();

			std::cout << "Got results for: " << "'Like'" << " in " << elapsed << " sec from startup!" <<std::endl;
			lock.unlock();

			x++;
		}
		cur_index.kill_mainloop();
	}
};



