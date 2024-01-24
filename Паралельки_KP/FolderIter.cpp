#include "FolderIter.h"

FolderIter::FolderIter(std::string new_path, int new_start, int new_end, std::string new_filename_end) 
{
	m_path = new_path;
	m_filename_end = new_filename_end;
	m_iter_start = new_start;
	m_iter_current = new_start;
	m_iter_end = new_end;
}

bool FolderIter::is_empty() const
{
	read_lock lock(m_rw_lock, std::defer_lock);
	if (m_iter_current > m_iter_end) {
		return true;
	}
	return false;
}

std::string FolderIter::try_get_filename()
{
	write_lock lock(m_rw_lock, std::defer_lock);

	if (lock.try_lock()) {

		//std::cout << "succsessfully aquired" << std::endl;

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

std::vector<std::string> FolderIter::get_filenames(int asked_qty)
{
	write_lock lock(m_rw_lock);
	return tu_get_filenames(asked_qty);
}

std::vector<std::string> FolderIter::try_get_filenames(int asked_qty)
{
	write_lock lock(m_rw_lock, std::defer_lock);

	if (lock.try_lock()) {
		return tu_get_filenames(asked_qty);
	}
	std::vector<std::string> result;
	result.push_back("LCKD");
	return result;
}

std::string FolderIter::make_filename(int num) const
{
	
	std::string result = m_path;
	result.append(std::to_string(num));
	result.append(m_filename_end);
	//std::cout << "made filename  " << result << std::endl;
	return result;
}

std::vector<std::string> FolderIter::tu_get_filenames(int asked_qty)
{
	std::vector<std::string> result;

	int left_names = m_iter_end - m_iter_current + 1;
	if (left_names <= 0) {
		result.push_back("EMPT");
	}
	if (left_names <= asked_qty) {
		asked_qty = left_names;
	}
	int starting_name = m_iter_current;
	m_iter_current += asked_qty;

	
	for (int i = starting_name; i < starting_name + asked_qty; i++) {
		result.push_back(make_filename(i));
	}
	return result;
}
