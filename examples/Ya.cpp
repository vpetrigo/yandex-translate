#include <fstream>
#include "Ya_translate.hpp"

using ya_tr = Ya_translate::Ya_tr;
using bad_apikey = Ya_translate::Bad_apikey;

int main() { 
    try {
        std::string api_k;
        std::ifstream ifs{ "api_key.txt" };
        std::string from, to;
        
        ifs >> api_k;
        
        ifs.close();
        
        curl_global_init(CURL_GLOBAL_ALL);
        
        ya_tr t{ api_k };
        
        auto langs = t.show_langs();
        
        for (auto& elem : langs) {
            std::cout << "from: " << elem.first << " to: " << elem.second << std::endl;
        }
        
        std::cout << "Enter the translation direction separated by space: " << std::endl;
        while (std::cin >> from >> to) {
            if (t.check_direction(from, to)) {
                std::cout << from << '-' << to << " exist" << std::endl;
            }
            else {
                std::cout << from << '-' << to << " does not exist" << std::endl;
            }
        }
        
        curl_global_cleanup();
        
        return 0;
    }
    catch (const bad_apikey& e) {
        std::cout << e.get_err() << std::endl;
        curl_global_cleanup();
        
        return 1;
    }
}
