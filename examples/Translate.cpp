#include "Ya_translate.hpp"
#include <fstream>

using ya_tr = Ya_translate::Ya_tr;
using bad_apikey = Ya_translate::Bad_apikey;

int main()
try {
    std::ifstream ifs{"api_key.txt"};
    std::string api_key;
    ifs >> api_key;
    
    ya_tr tr{api_key};
    std::string from, to;
    std::string word;
    
    ifs.close();
    
    curl_global_init(CURL_GLOBAL_ALL);
    std::cout << "Enter the language code of translation (like en ru): ";
    std::cin >> from >> to;
    
    tr.set_language(from, to);
    
    std::cout << "Enter the word you want to translate: ";
    std::cin >> word;
    std::cout << tr.translate(word) << std::endl;   
    
    curl_global_cleanup();
    
    return 0;
}
catch (const bad_apikey& e) {
    std::cout << e.get_err() << std::endl;
    curl_global_cleanup();
    
    return 1;
}