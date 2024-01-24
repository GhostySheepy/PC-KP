#pragma once
#include <set>
#include <utility>
#include <unordered_map>
#include <string>
#include <future>

#include "external\Stemming\english_stem.h"

#include "FolderManager.h"
#include "ThreadPool.h"
#include "Helpers.h"

#include "external\boost\algorithm\string.hpp"
#include "external\boost\algorithm\string\classification.hpp"
#include "external\boost\algorithm\string\split.hpp"

using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

class InvertedIndex
{
private:
	stemming::english_stem<> StemEnglish;

	std::unordered_map<std::wstring, std::set<std::string>> m_index;
	mutable read_write_lock m_index_lock;


	FolderManager& m_FolderManager;
	bool indexed; 
	std::atomic<int> m_ready_count;

	std::vector<std::pair< std::string, std::promise<std::set<std::string>>&>> m_find_reqests;
	mutable read_write_lock m_req_lock;

	Thread_Pool<std::function<void()>>& m_ThreadPool;

	bool escape;


public:
	InvertedIndex(FolderManager& new_FM, Thread_Pool<std::function<void()>>& new_TM);

	int mainloop();
	void kill_mainloop();

	bool try_index_syncro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index);
	void force_index_synchro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index);
	void print_index();

	void find_request(std::string req_word, std::promise<std::set<std::string>>& promise);

	

	std::set<std::string> apply_find(std::string rq);
	

private:
	void indexing_loop(int worker_num);
	void start_indexing();

	void tu_index_synchro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index);


};

