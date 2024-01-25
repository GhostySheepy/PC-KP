#pragma once
#include "InvertedIndex.h"
namespace Workers {
	void file_worker(int worker_num, InvertedIndex* cur_ind, FolderManager& cur_fm, std::atomic<int>& ready_count) {
		stemming::english_stem<> StemEnglish;
		std::unordered_map<std::wstring, std::set<std::string>> mini_index;
		int skipped_index_synchro = 0;

		bool stop_work_add = false;
		std::vector<std::string> work_files = cur_fm.get_filenames(100);
		if (work_files[0] == "EMPT") {
			work_files = {};
			stop_work_add = true;
		}

		std::string cur_file;
		std::string cur_file_readout;
		std::set<std::string> cur_words;

		std::wstring proc_word;

		while (work_files.size() > 0) {
			//index
			cur_file = work_files.back();
			work_files.pop_back();
			cur_file_readout = Helpers::readfile(cur_file);

			boost::split(cur_words, cur_file_readout, boost::is_any_of("?!:., \n"));

			for (std::string word : cur_words) {
				if (word.size() < 2) {
					continue;
				}

				boost::to_lower(word);
				proc_word = Helpers::string_to_wstring(word);
				
				StemEnglish(proc_word);

				if (mini_index.find(proc_word) == mini_index.end()) {
					mini_index[proc_word] = std::set<std::string>();
				}

				mini_index[proc_word].insert(cur_file);

			}

			//synchronize
			skipped_index_synchro++;
			if (skipped_index_synchro > 5) {
				bool synchro_result = cur_ind->try_index_syncro(mini_index);
				if (synchro_result == true) {
					//Helpers::print_index(mini_index, "mini 0");

					mini_index = {};
					skipped_index_synchro = 0;
				}
			}
			else if (skipped_index_synchro > 15) {
				cur_ind->force_index_synchro(mini_index);
				//Helpers::print_index(mini_index, "mini 1");
				mini_index = {};
				skipped_index_synchro = 0;
			}

			//replenish work
			if (stop_work_add == true) {
				continue;
			}

			if (work_files.size() > 30 && work_files.size() % 10 == 0) {
				std::vector<std::string> try_add = cur_fm.try_get_filenames(100);
				if (try_add[0] == "EMPT") {
					stop_work_add = true;
				}
				else if (try_add[0] != "LCKD") {
					for (auto i : try_add) {
						work_files.push_back(i);
					}
				}
			}
			else if (work_files.size() < 10) {
				std::vector<std::string> try_add = cur_fm.get_filenames(100);
				if (try_add[0] == "EMPT") {
					stop_work_add = true;
				}
				else if (try_add[0] != "LCKD") {
					for (auto i : try_add) {
						work_files.push_back(i);
					}
				}
			}

		}

		if (!mini_index.empty()) {
			cur_ind->force_index_synchro(mini_index);
		}

		ready_count++;
	}

	void search_worker(InvertedIndex* cur_ind, std::pair< std::string, std::promise<std::set<std::string>>&> cur_req) {
		std::set<std::string> res = cur_ind->apply_find(cur_req.first);
		cur_req.second.set_value(res);
	}
};

