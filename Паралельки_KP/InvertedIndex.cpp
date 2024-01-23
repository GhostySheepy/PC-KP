#include "InvertedIndex.h"

InvertedIndex::InvertedIndex(FolderManager& new_FM, Thread_Pool<std::function<void()>>& new_TM):
	m_FolderManager(new_FM),
	m_ThreadPool(new_TM),
	indexed(false)
{
	start_indexing();
}

int InvertedIndex::mainloop()
{
	if (indexed == false) {
		if (m_ThreadPool.queue_size() == 0) {
			indexed = true;
		}
	}
	return 0;
}

void InvertedIndex::indexing_loop()
{
	std::unordered_map<std::wstring, std::set<std::string>> mini_index;
	int skipped_index_synchro = 0;
	std::vector<std::string> work_files = m_FolderManager.get_filenames(5);
	bool stop_work_add = false;
	std::string cur_file;
	std::string cur_file_readout;
	std::set<std::string> cur_words;

	std::wstring proc_word;

	while (work_files.size() > 0) {
		//index
		cur_file = work_files.back();
		work_files.pop_back();
		cur_file_readout = Helpers::readfile(cur_file);
		
		boost::split(cur_words, cur_file_readout, boost::is_any_of("#&?!:;., \r\n\t"), boost::token_compress_on);
		
		for (std::string word : cur_words) {
			if (word.size() < 2) {
				continue;
			}

			proc_word = Helpers::string_to_wstring(word);
			boost::to_lower(proc_word);
			StemEnglish(proc_word);

			if (mini_index.find(proc_word) == mini_index.end()) {
				mini_index[proc_word] = std::set<std::string>();
			}

			mini_index[proc_word].insert(cur_file);
			
		}

		//synchronize
		if (skipped_index_synchro < 5) {
			bool synchro_result = try_index_syncro(mini_index);
			if (synchro_result == false) {
				skipped_index_synchro++;
			}
			else {
				Helpers::print_index(mini_index, "mini 0");

				mini_index = {};
				skipped_index_synchro = 0;
			}

		}
		else {
			force_index_synchro(mini_index);
			Helpers::print_index(mini_index, "mini 1");
			mini_index = {};
			skipped_index_synchro = 0;
		}

		//replenish work
		if (stop_work_add == true){
			continue;
		}

		if (work_files.size() > 3) {
			std::string try_add = m_FolderManager.try_get_filename();
			if (try_add == "EMPT") {
				stop_work_add = true;
			}
			else {
				work_files.push_back(try_add);
			}
		}
		else {
			std::vector<std::string> try_add = m_FolderManager.get_filenames(5);
			if (try_add[0] == "EMPT") {
				stop_work_add = true;
			}
			else {
				for (auto i : try_add) {
					work_files.push_back(i);
				}
			}
		}

	}

	if (!mini_index.empty()) {
		force_index_synchro(mini_index);
	}


		
}

void InvertedIndex::start_indexing()
{
	FolderIter* new_folder = new FolderIter("test/", 0, 0, ".txt");
	m_FolderManager.add_folder(new_folder);

	indexing_loop();

	new_folder = new FolderIter("test/", 1, 1, ".txt");
	m_FolderManager.add_folder(new_folder);

	indexing_loop();
	Helpers::print_index(m_index, "maxi");

}

bool InvertedIndex::try_index_syncro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index)
{
	write_lock lock(m_index_lock, std::defer_lock);
	if (lock.try_lock()) {
		tu_index_synchro(mini_index);
		return true;
	}
	return false;
}

void InvertedIndex::force_index_synchro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index)
{
	write_lock lock(m_index_lock);
	tu_index_synchro(mini_index);
}

void InvertedIndex::tu_index_synchro(std::unordered_map<std::wstring, std::set<std::string>>& mini_index)
{
	
	for (auto i : mini_index) {
		if (m_index.find(i.first) == m_index.end()) {
			m_index[i.first] = i.second;
		}
		else {
			m_index[i.first].merge(i.second);
		}
	}
}
