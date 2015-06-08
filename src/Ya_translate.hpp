#include <curl/curl.h>
#include "json.hpp"

namespace Ya_translate {
    using json = nlohmann::json;
    
    class Bad_apikey {
    public:
        Bad_apikey() {}
        Bad_apikey(const std::string& s) : err{s} {}
        
        const std::string& get_err() const {
            return err;
        }
    private:
        std::string err;
    };
    
    class Ya_tr {
    public:
        explicit Ya_tr(const std::string& a_k) : api_key{a_k} {
            if (!api_key.size()) {
                throw Bad_apikey{"You did not pass your API-key"};
            }
            
            ya_h = curl_easy_init();
            curl_easy_setopt(ya_h, CURLOPT_URL, get_langs_link.c_str());
            std::string key_f = (static_cast<std::string> ("key=") + a_k);
            curl_easy_setopt(ya_h, CURLOPT_POSTFIELDS, key_f.c_str());
            curl_easy_setopt(ya_h, CURLOPT_CAINFO, R"(C:\msys64\usr\ssl\certs\ca-bundle.crt)"); // Need to be a path with certificate for https session
            CURLcode res = curl_easy_perform(ya_h);
            
            if (res != CURLE_OK) {
                std::cout << "Something went wrong\n";
            }
        }
        
        ~Ya_tr() {
            curl_easy_cleanup(ya_h);
        }
         
        static const std::string get_langs_link;
        static const std::string detec_lang_link;
        static const std::string translate_link;
    private:
        const std::string& api_key;
        CURL *ya_h;
    };
}
