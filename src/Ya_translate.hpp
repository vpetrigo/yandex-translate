#include <curl/curl.h>
#include <stdexcept>
#include "json.hpp"

namespace Ya_translate {
    using json = nlohmann::json;
    
    // Define standart json quotes that can be found in responses
    // {
    //      "dirs" : "en-ru", "ru-fr", ... <- here we have array of strings
    //      "code" : 200
    // }
    constexpr char json_quotes = '\"';
    
    // Class for throwing error if something wrong with an API-key
    class Bad_apikey {
     public:
        Bad_apikey() : err{"Bad API-key"} {}
        Bad_apikey(const std::string& s) : err{s} {}
        
        const std::string& get_err() const {
            return err;
        }
     private:
        std::string err;
    };
    
    // Class for working with Yandex Translator API
    class Ya_tr {
     public:
        explicit Ya_tr(const std::string& a_k);
        
        ~Ya_tr() {
            curl_easy_cleanup(ya_h);
        }
        
        static const std::string get_langs_link;
        static const std::string detec_lang_link;
        static const std::string translate_link;
        static const std::vector<std::string> resp_codes;
        
        // NAME show_langs()
        // DESCRIPTION Provide data about all available data for user
        const std::vector<std::pair<std::string, std::string>>& show_langs() const {
            return avail_lang;
        }
        
        // NAME check_direction()
        // DESCRIPTION Check if it is possible to use selected direction
        // Returns true - if it is possible, false - otherwise
        bool check_direction(const std::string& from, const std::string& to) {
            if (!from.size() || !to.size()) {
                throw std::logic_error("language direction was not passed");
            }
            
            if (from.size() > 2 || to.size() > 2) {
                throw std::logic_error("should pass only code of the language (\"en\", \"ge\", etc.)");
            }
            
            for (auto& i : avail_lang) {
                if (i.first == from && i.second == to) {
                    return true;
                }
            }
            
            return false;
        }
        
     private:
        static size_t handle_data(void *buffer, size_t size, size_t nmemb, void *userp);      
        std::vector<std::pair<std::string, std::string>> get_langs(const json::value_type& dirs, const char delim);

        // NAME check_error_code()
        // DESCRIPTION This function whether a responce from server contain
        // error code
        bool check_error_code(const json::value_type &resp) const;

        const std::string api_key;
        char lang_delim { '-' };
        // Storage of data comes from API responses
        json data;
        // Vector of all available language directions for Yandex Translate { {"en", "ru"}, {"ar", "ua"} ... }
        std::vector<std::pair<std::string, std::string>> avail_lang;
        CURL *ya_h;
    };
}
