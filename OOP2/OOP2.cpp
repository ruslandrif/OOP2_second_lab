#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include "TextClassifier.h"


std::list<std::pair<N_grams,std::string>> Categories_tables;

int main()
{


    const std::string input_dir_filename = "InputTexts"; //name of the directory with the input files to classificate (may be absolute path to some folder)

    const std::string categories_dir_filename = "Categories";  //name of the directory with the categories files (may be absolute path to some folder)
  
    

    TextClassifier tc(Categories_tables, input_dir_filename, categories_dir_filename);


    tc.info_about_all_texts();
    return 0;
}





