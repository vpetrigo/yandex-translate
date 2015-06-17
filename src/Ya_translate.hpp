#include <curl/curl.h>
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
     private:
        static size_t handle_data(void *buffer, size_t size, size_t nmemb, void *userp);      
        std::vector<std::pair<std::string, std::string>> get_langs(const json::value_type& dirs, const char delim);

        // NAME check_error_code()
        // DESCRIPTION This function whether a responce from server contain
        // error code
        bool check_error_code(const json::value_type &resp) const;

        const std::string api_key;
        char lang_delim { '-' };
        json data;
        // Vector of all available language directions for Yandex Translate { {"en", "ru"}, {"ar", "ua"} ... }
        std::vector<std::pair<std::string, std::string>> avail_lang;
        CURL *ya_h;
    };
}
