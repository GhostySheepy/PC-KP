#pragma once

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <mutex>
#include <shared_mutex>

using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

class FolderIter
{
private:
	std::string m_path;

	int m_iter_start;
	int m_iter_current;
	int m_iter_end;

	std::string m_filename_end;

	mutable read_write_lock m_rw_lock;
public:


	FolderIter(std::string new_path, int new_start, int new_end, std::string new_filename_end);

	bool is_empty() const;

	std::string try_get_filename();
	std::vector<std::string> get_filenames(int asked_qty);
	std::vector<std::string> try_get_filenames(int asked_qty);

private:

	std::string make_filename(int num) const;

	std::vector<std::string> tu_get_filenames(int asked_qty);
};

