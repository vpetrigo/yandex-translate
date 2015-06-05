#include <fstream>
#include <curl/curl.h>
#include "Ya_translate.hpp"

using ya_tr = Ya_translate::Ya_tr;
using bad_apikey = Ya_translate::Bad_apikey;

int main() { 
    try {
        std::string api_k;
        std::ifstream ifs{ "api_key.txt" };
        
        ifs >> api_k;
        
        curl_global_init(CURL_GLOBAL_ALL);
        
        ya_tr t{api_k};
        
        return 0;
    }
    catch (const bad_apikey& e) {
        std::cout << e.get_err() << std::endl;
    }
}
