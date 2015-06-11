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
        explicit Ya_tr(const std::string& a_k) : api_key{a_k} {
            if ( !api_key.size() ) {
                throw Bad_apikey{"You did not pass your API-key"};
            }
            
            ya_h = curl_easy_init();
            curl_easy_setopt(ya_h, CURLOPT_URL, get_langs_link.c_str());
            std::string key_f = (static_cast<std::string> ("key=") + a_k);
            curl_easy_setopt(ya_h, CURLOPT_POSTFIELDS, key_f.c_str());
            curl_easy_setopt(ya_h, CURLOPT_CAINFO, R"(..\certs\ca-bundle.crt)"); // Need to be a path with the certificate for https session
            curl_easy_setopt(ya_h, CURLOPT_WRITEFUNCTION, handle_data);
            curl_easy_setopt(ya_h, CURLOPT_WRITEDATA, this);
            CURLcode res = curl_easy_perform(ya_h);
            
            if ( res != CURLE_OK ) {
                std::cout << "Something went wrong\n";
            }
            
            avail_lang = get_langs(data["dirs"], lang_delim);
            
            for (auto i : avail_lang) {
                if (i.first == "ru") {
                    std::cout << "from: " << i.first << " to: "
                        << i.second << std::endl;
                }
            }
        }
        
        ~Ya_tr() {
            curl_easy_cleanup(ya_h);
        }
        
        static const std::string get_langs_link;
        static const std::string detec_lang_link;
        static const std::string translate_link;
    private:
        static size_t handle_data(void *buffer, size_t size, size_t nmemb, void *userp);      
        std::vector<std::pair<std::string, std::string>>& get_langs(json::value_type& dirs, const char delim);
        
        const std::string api_key;
        char lang_delim { '-' };
        json data;
        std::vector<std::pair<std::string, std::string>> avail_lang;
        CURL *ya_h;
    };
}
