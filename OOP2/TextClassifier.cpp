#include "TextClassifier.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <string>
#include <utility>
#include <unordered_map>
unsigned my_count(const N_grams& n, std::pair<std::wstring, unsigned> val) { //count the number of occurencies of the string from val in the table of n-grams
	unsigned res = 0;                                                        //helping function
	for (auto& i : n) {
		if (i.first == val.first)
			++res;
	}
	return res;
}


//using N_grams = std::vector<std::pair<std::wstring, unsigned>>;

void TextClassifier::remove_N_grams_duplicates_and_sort(std::list<std::pair<N_grams, std::string>>& l) {

	//int count = 0;
	for (auto& ng : l) {
		count.clear();
		//std::cout << count++ << std::endl;
		for (auto& i : ng.first) {
			++count[i.first];
		}
		std::sort(ng.first.begin(), ng.first.end(), [&ng,this](const std::pair<std::wstring, unsigned>& f, const std::pair<std::wstring, unsigned>& s) {
			return count[f.first] < count[s.first];
		
		}); //sorting by number of occurencies

		for (auto& i : ng.first) {
			i.second = count[i.first];
		}

		ng.first.erase(std::unique(ng.first.begin(), ng.first.end()), ng.first.end()); //remove duplicates
	}

}



N_grams TextClassifier::create_N_Grams(const std::string& from, const std::string& to) { //create N-grams for file "from" and write them to file "to"

	N_grams result;

	std::wstring w;

	std::wifstream file(from,std::fstream::binary);
	if (!file.is_open()) throw std::filesystem::filesystem_error(std::string("invalid input file"), std::error_code::error_code());

	wchar_t current_symbol;

	while(!file.eof()) {
		file.read(&current_symbol, 1);

		if (iswspace(current_symbol) && !w.empty()) {
			
				
				w.push_back(wchar_t('_'));


				for (unsigned i = LOWER_NGRAM_LEN; i <= UPPER_NGRAM_LEN && i <= w.size(); ++i) {
					for (unsigned j = 0; j <= w.size() - i; ++j) {
						//std::wstring N_gram = w.substr(j, i);
						
						result.push_back({ w.substr(j, i),0});
						
					}
				}
			
			w.clear();
			w.push_back(wchar_t('_'));
		}

		else 
			w.push_back(current_symbol);
		
	}

	return result;
}


TextClassifier::TextClassifier(){

}

TextClassifier::TextClassifier(const std::list<std::pair<N_grams, std::string>>& lst, const std::string& input_texts_directory,const std::string& categories_directory) {
	l = lst;
	input_directory = input_texts_directory;
	this->categories_directory = categories_directory;

	create_categories_table();
}


std::string TextClassifier::Classificate_file(const std::string& filename) {
	N_grams grams;
	try {
		grams = create_N_Grams(filename, filename + "_N_grams.txt");
	}
	catch (const std::filesystem::filesystem_error& f) {
		throw std::filesystem::filesystem_error("invalid file: " + filename, std::error_code::error_code());
	}

	this->count.clear();
	for (auto& i : grams) {
		++count[i.first];
	}
	

	std::sort(grams.begin(), grams.end(), [&grams,this](const std::pair<std::wstring, unsigned>& f, const std::pair<std::wstring, unsigned>& s) {
		return count[f.first] < count[s.first];

		});

	for (auto& i : grams) {
		i.second = count[i.first];                                   //creating n-gram table for input file, sort it and remove duplicates
	}

	grams.erase(std::unique(grams.begin(), grams.end()), grams.end());

	int min_distance = INT_MAX;
	std::string matching_file;

	
	for (auto& category : l) { //traverse through all categories
		
		int curr_distance = 0;

		int count = 0;
		
		for (auto& n_gram : grams) {
			//N_grams ng = category.first;

			auto _find = std::find_if(category.first.begin(), category.first.end(), [&n_gram](const std::pair<std::wstring, unsigned>& n) {return n.first == n_gram.first; });

			curr_distance += _find == category.first.end() ? //that n-gram exist in the category profile? 
				max_distance : //add max distance, if not
				_find - category.first.begin() - count; //if exist, add the length between them
			++count;
		}

		if (curr_distance < min_distance) {
			min_distance = curr_distance;
			matching_file = category.second;
		}
		
	}

	return matching_file; //return path to the most matching file
}

void TextClassifier::create_categories_table() {
	l.clear();
	try {
		for (auto& i : std::filesystem::directory_iterator(categories_directory)) {
			const std::filesystem::path curr_p = i.path();

			

			l.push_back({ create_N_Grams(curr_p.string(), curr_p.filename().string()) ,curr_p.string() }); //create tables of n-grams for all files
		}
	}
	catch (const std::filesystem::filesystem_error& fs) {
		throw std::invalid_argument("categories directory is invalid");
	}

	remove_N_grams_duplicates_and_sort(l);


	int max_size_of_the_table = 0; //calculating max distance

	for (auto& i : l) {
		if (i.first.size() > max_size_of_the_table)
			max_size_of_the_table = i.first.size();
	}
	max_distance = max_size_of_the_table;
}

void TextClassifier::info_about_all_texts() {
	try {
		for (auto& dir : std::filesystem::directory_iterator(input_directory)) {
			std::cout << "The most matching text for " << dir.path().string() << ": " << Classificate_file(dir.path().string()) << std::endl;
		}
	}
	catch (const std::filesystem::filesystem_error) {
		throw std::invalid_argument("input directory is invalid");
	}
}