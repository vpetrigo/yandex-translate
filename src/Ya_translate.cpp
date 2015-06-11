#include "Ya_translate.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Ya_translate {
    using json = nlohmann::json;
    
    /* Function prototypes */
    
    // An available language response from Yandex Translator is like that :
    // en-fr, en-ge, en-ru ...
    // So, here we need to split it by a delimiter (in the case above the delimiter is '-')
    static std::pair<std::string, std::string> split_lang(const std::string& langs, const char delim);
    
    // Function for deleting quotes from json responce
    // Accept json string, for example "english-russian" and delete \" quote from the string
    static void delete_quotes(std::string& json_str);
        
    /* Variables */
    
    // The API link for getting available translation directions
    const std::string Ya_tr::get_langs_link = "https://translate.yandex.net/api/v1.5/tr.json/getLangs";
    // The API link for detecting language of passed string
    const std::string Ya_tr::detec_lang_link = "https://translate.yandex.net/api/v1.5/tr.json/detect";
    // The API link for getting translation
    const std::string Ya_tr::translate_link = "https://translate.yandex.net/api/v1.5/tr.json/translate";
    
    /* Definitions */
    
    size_t Ya_tr::handle_data(void *buffer, size_t size, size_t nmemb, void *userp) {
        json data;
        std::istringstream buf { static_cast<char *> (buffer) };
        
        buf >> data;
        
        static_cast<Ya_tr *> (userp)->data = data;
        
        return size * nmemb;
    }
    
    
    std::vector<std::pair<std::string, std::string>>& Ya_tr::get_langs(json::value_type& dirs, const char delim) {
            std::vector<std::pair<std::string, std::string>> *langs = new std::vector<std::pair<std::string, std::string>>;
            
            for (auto elem : dirs) {
                std::string from_to { elem.dump() };
                
                delete_quotes(from_to);                
                langs->push_back(split_lang(from_to, delim));
            }
            
            return *langs;
        }
    
    
    static std::pair<std::string, std::string> split_lang(const std::string& langs, const char delim) {      
        std::string from;
        std::string to;
        
        int delim_pos = s.find(delim);
        
        from = s.substr(0, delim_pos);
        to = s.substr(delim_pos + 1);
        
        return std::pair<std::string, std::string> {from, to};
    }
    
    static void delete_quotes(std::string& json_str) {      
        json_str.erase(std::remove(json_str.begin(), json_str.end(), json_quotes), json_str.end());
    }
}
