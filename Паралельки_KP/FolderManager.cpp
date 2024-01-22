#include "FolderManager.h"

FolderManager::FolderManager(std::vector<FolderIter*> new_folders):
	m_folders(new_folders)
{
}

std::string FolderManager::try_get_filename()
{
	std::string result;
	for (int i = 0; i < m_folders.size(); i++) {
		result = m_folders[i]->try_get_filename();
		if (result != "EMPT" && result != "LCKD") {
			break;
		}

	}
	return result;
}

std::vector<std::string> FolderManager::get_filenames(int requested_num)
{
	std::vector<std::string> result;
	bool found = false;
	for (int i = 0; i < m_folders.size(); i++) {
		result = m_folders[i]->try_get_filenames(requested_num);
		if (result[0] != "EMPT" && result[0] != "LCKD") {
			found = true;
			break;
		}

	}
	if (found != true) {

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(0, m_folders.size()-1);

		result = m_folders[dist(mt)]->get_filenames(requested_num);
	}

	return result;
}
