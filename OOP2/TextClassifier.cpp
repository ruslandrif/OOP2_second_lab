#include "TextClassifier.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <string>
#include <utility>

unsigned my_count(const N_grams& n, std::pair<std::wstring, unsigned> val) { //count the number of occurencies of the string from val in the table of n-grams
	unsigned res = 0;                                                        //helping function
	for (auto& i : n) {
		if (i.first == val.first)
			++res;
	}
	return res;
}


//using N_grams = std::vector<std::pair<std::wstring, unsigned>>;

void remove_N_grams_duplicates_and_sort(std::list<std::pair<N_grams, std::string>>& l) {
	
	//int count = 0;
	for (auto& ng : l) {
		
		//std::cout << count++ << std::endl;
		std::sort(ng.first.begin(), ng.first.end(), [ng](const std::pair<std::wstring, unsigned>& f, const std::pair<std::wstring, unsigned>& s) {
			return my_count(ng.first, f) < my_count(ng.first, s);
		
		}); //sorting by number of occurencies

		for (auto& i : ng.first) {
			i.second = my_count(ng.first, i); //remember number of occurencies
		}

		ng.first.erase(std::unique(ng.first.begin(), ng.first.end()), ng.first.end()); //remove duplicates
	}

}



N_grams create_N_Grams(const std::string& from, const std::string& to) { //create N-grams for file "from" and write them to file "to"

	N_grams result;

	std::wstring w;

	std::wifstream file(from,std::fstream::binary);

	wchar_t current_symbol;

	w.clear();

	while(!file.eof()) {
		file.read(&current_symbol, 1);

		if (iswspace(current_symbol)) {
			if (!w.empty()) {
				
				w.push_back(wchar_t('_'));

				std::wstring w2;
				w2.push_back(wchar_t('_'));  //weird adding _, but std::basic_string::insert did not work for me for some reason
				w2 += w;
				w = w2;

				for (int i = LOWER_NGRAM_LEN; i <= UPPER_NGRAM_LEN && i <= w.size(); ++i) {
					for (int j = 0; j <= w.size() - i; j += 1) {
						std::wstring N_gram = w.substr(j, i);
						
						result.push_back({ N_gram,0});
						
					}
				}
			}
			w.clear();
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
	auto grams = create_N_Grams(filename, filename + "_N_grams.txt");

	std::sort(grams.begin(), grams.end(), [grams](const std::pair<std::wstring, unsigned>& f, const std::pair<std::wstring, unsigned>& s) {
		return my_count(grams, f) < my_count(grams, s);

		});

	for (auto& i : grams) {
		i.second = my_count(grams, i);                                   //creating n-gram table for input file, sort it and remove duplicates
	}

	grams.erase(std::unique(grams.begin(), grams.end()), grams.end());

	int min_distance = INT_MAX;
	std::string matching_file;

	
	for (auto& category : l) { //traverse through all categories
		
		int curr_distance = 0;

		int count = 0;
		
		for (auto& n_gram : grams) {
			N_grams ng = category.first;

			curr_distance += std::find_if(ng.begin(), ng.end(), [n_gram](const std::pair<std::wstring, unsigned>& n) {return n.first == n_gram.first; }) == ng.end() ? //that n-gram exist in the category profile? 
				max_distance : //add max distance, if not
				(std::find_if(ng.begin(), ng.end(), [n_gram](const std::pair<std::wstring, unsigned>& n) {return n.first == n_gram.first; }) - ng.begin()) - count; //if exist, add the length between them
			count++;
		}

		if (curr_distance < min_distance) {
			min_distance = curr_distance;
			matching_file = category.second;
		}
		
	}

	return matching_file; //return path to the most matching file
}

void TextClassifier::create_categories_table() {
	for (auto& i : std::filesystem::directory_iterator(categories_directory)) {
		const std::filesystem::path curr_p = i.path();

		auto lst = create_N_Grams(curr_p.string(), curr_p.filename().string());

		l.push_back({ lst ,curr_p.string() }); //create tables of n-grams for all files
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

	for (auto& dir : std::filesystem::directory_iterator(input_directory)) {
		std::cout <<"The most matching text for " << dir.path().string() << ": "<< Classificate_file(dir.path().string()) << std::endl;
	}
}