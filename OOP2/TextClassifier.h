#pragma once
#include <string>
#include <vector>
#include <list>

using N_grams = std::vector<std::pair<std::wstring,unsigned>>;


//#define MAX_SCORE 150

#define LOWER_NGRAM_LEN	2
#define UPPER_NGRAM_LEN	5



N_grams create_N_Grams(const std::string& from, const std::string& to); //create N-grams for file "from" and write them to file "to"

void remove_N_grams_duplicates_and_sort(std::list<std::pair<N_grams, std::string>>& l); //sort N-grams list from less to most frequent n-gram,
																						// remove duplicates, and calculating number of occurences



class TextClassifier
{
public:
	TextClassifier(const std::list<std::pair<N_grams, std::string>>& lst,const std::string& input_texts_directory,const std::string& categories_texts_directory);

	TextClassifier();


	
	std::string Classificate_file(const std::string& filename); //classificate file. return path to the most matching file

	void create_categories_table(); //create the list of categories N-grams tables

	void info_about_all_texts(); //prints path to the most matching text of each input file
private:
	std::string input_directory;   //input texts directory

	std::string categories_directory; //directory with the categories files


	std::list<std::pair<N_grams, std::string>> l; //list with the n-grams tables


	long max_distance; //we will calculate it after creating table of categories
};

