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

class FolderDefiner {
private:
	const std::string m_path;

	const int m_iter_start;
	int m_iter_current;
	const int m_iter_end;
	const std::string m_filename_end;

	mutable read_write_lock m_rw_lock;

public:
	FolderDefiner(FolderDefiner&&) = default;
	FolderDefiner(const FolderDefiner&) = default;

	FolderDefiner(std::string new_path, int new_start_iter, int new_end_iter, std::string new_filename_end) :
		m_path(new_path),
		m_filename_end(new_filename_end),
		m_iter_start(new_start_iter),
		m_iter_current(new_start_iter),
		m_iter_end(new_end_iter)
	{}

	bool is_empty() {
		read_lock lock(m_rw_lock, std::defer_lock);
		if (m_iter_current > m_iter_end) {
			return true;
		}
		return false;
	}
	std::string try_get_single_filename() {
		write_lock lock(m_rw_lock, std::defer_lock);

		if (lock.try_lock()) {

			std::cout << "succsessfully aquired" << std::endl;

			if (m_iter_current > m_iter_end) {
				return "EMPT";
			}
			int name_int = m_iter_current;
			m_iter_current++;
			lock.unlock();

			return make_filename(name_int);
		}
		return "LCKD";
	}

	std::vector<std::string> get_filenames(int asked_qty) {

		write_lock lock(m_rw_lock);
		int left_names = m_iter_end - m_iter_current + 1;
		if (left_names <= asked_qty) {
			asked_qty = left_names;
		}
		int starting_name = m_iter_current;
		m_iter_current += asked_qty;
		lock.unlock();

		std::vector<std::string> result(asked_qty);
		for (int i = starting_name; i < starting_name + asked_qty; i++) {
			result.push_back(make_filename(i));
		}
		return result;
		
	}
private:
	std::string make_filename(int num) const {
		std::string result = m_path;
		result.append(std::to_string(num));
		result.append(m_filename_end);
		return result;
	}
};
	
class FolderIterator
{
	FolderIterator(std::vector<std::vector<std::string>>);

};

