#pragma once
#include <set>
#include <unordered_map>
#include <string>

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

	Thread_Pool<std::function<void()>>& m_ThreadPool;


public:
	InvertedIndex(FolderManager& new_FM, Thread_Pool<std::function<void()>>& new_TM);

	int mainloop();


private:
	void indexing_loop();
	void start_indexing();


	bool try_index_syncro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index);
	void force_index_synchro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index);

	void tu_index_synchro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index);


};

