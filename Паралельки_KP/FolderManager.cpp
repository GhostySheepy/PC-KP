#include "FolderManager.h"

FolderManager::FolderManager(std::vector<FolderIter*> new_folders):
	m_folders(new_folders),
	m_empty(false)
{
}

void FolderManager::add_folder(FolderIter* new_folder)
{
	m_folders.push_back(new_folder);
}

std::string FolderManager::try_get_filename()
{
	std::string result;
	int left_cap = m_folders.size();
	for (int i = 0; i < m_folders.size(); i++) {
		result = m_folders[i]->try_get_filename();
		if (result != "EMPT" && result != "LCKD") {
			break;
		}
		if (result == "EMPT") {
			left_cap -= 1;
		}

	}

	if (left_cap == 0) {
		m_empty = true;
	}
	return result;
}

std::vector<std::string> FolderManager::get_filenames(int requested_num)
{
	std::vector<std::string> result;
	bool found = false;
	int left_cap = m_folders.size();

	for (int i = 0; i < m_folders.size(); i++) {
		result = m_folders[i]->try_get_filenames(requested_num);
		if (result[0] == "EMPT") {
			left_cap -= 1;
		}
		if (result[0] != "EMPT" && result[0] != "LCKD") {
			found = true;
			break;
		}
		

	}

	if (left_cap == 0) {
		m_empty = true;
		return result;
	}

	if (found != true) {

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(0, m_folders.size()-1);

		result = m_folders[dist(mt)]->get_filenames(requested_num);
	}

	return result;
}

bool FolderManager::is_empty()
{
	return m_empty;
}
