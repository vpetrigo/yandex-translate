#ifndef YANDEX_TRANSLATE_HPP
#define YANDEX_TRANSLATE_HPP

#include <curl/curl.h>
#include <stdexcept>
#include "json.hpp"

namespace Ya_translate {
    using json = nlohmann::json;
    
    // Define standard json quotes that can be found in responses
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
        
        // The API link for getting available translation directions
        static const std::string get_langs_link;
        // The API link for detecting language of passed string
        static const std::string detec_lang_link;
        // The API link for getting translation
        static const std::string translate_link;
        // Response codes from API
        static const std::vector<int> resp_codes;
        
        // Position of translated text in Yandex Translate API
        // Example: {"code":200,"lang":"en-ru","text":[...]}
        static constexpr int translate_pos = 0;
        
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
        
        // NAME set_language()
        // DESCRIPTION sets up language direction which should be used for further
        // translations
        bool set_language(const std::string& from, const std::string& to) {
            if (!check_direction(from, to)) {
                return false;
            }
            else {
                from_to = std::make_pair(from, to);
                return true;
            }
        }
        
        // NAME translate()
        // DESCRIPTION translates an input string according to setted up language direction
        std::string translate(const std::string& s);
        
     private:
        static size_t handle_data(void *buffer, size_t size, size_t nmemb, void *userp);      
        std::vector<std::pair<std::string, std::string>> get_langs(const json::value_type& dirs, const char delim);

        // NAME check_error_code()
        // DESCRIPTION This function whether a response from server contain
        // error code
        bool check_error_code(const json::value_type &resp) const;
        
        // The string to store passed API-key
        const std::string api_key;
        // Translation direction language delimiter used in Yandex Translate API
        char lang_delim { '-' };
        // Storage of data comes from API responses
        json data;
        // Vector of all available language directions for Yandex Translate { {"en", "ru"}, {"ar", "ua"} ... }
        std::vector<std::pair<std::string, std::string>> avail_lang;
        std::pair<std::string, std::string> from_to;
        CURL *ya_h;
    };
}

#endif // end of YANDEX_TRANSLATE_HPP
