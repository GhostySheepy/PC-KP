#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <mutex>
#include <shared_mutex>

#include <random>


#include "FolderIter.h"

using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

class FolderManager
{
private:
	std::vector<FolderIter*> m_folders;

	bool m_empty;
public:
	FolderManager(std::vector<FolderIter*> new_folders);
	
	void add_folder(FolderIter* new_folder);

	std::string try_get_filename();
	std::vector<std::string> try_get_filenames(int requested_num);

	std::vector<std::string> get_filenames(int requested_num);
	bool is_empty();
};

