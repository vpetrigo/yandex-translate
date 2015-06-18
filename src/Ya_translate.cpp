#include "Ya_translate.hpp"

namespace Ya_translate {
    using json = nlohmann::json;
    
    /* Function prototypes */
    
    // An available language response from Yandex Translator is like that :
    // en-fr, en-ge, en-ru ...
    // So, here we need to split it by a delimiter (in the case above the delimiter is '-')
    static std::pair<std::string, std::string> split_lang(const std::string& langs, const char delim);
    
    static void handle_curl_err(const std::string& msg);
    static std::string err_msg_create(const std::string& err_msg);
    
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
    // Response codes from API
    const std::vector<int> Ya_tr::resp_codes {
                                              200,  // OK
                                              401,  // Different error codes
                                              402,  // that might be in a responce from
                                              403,  // YA API in the "code" field
                                              404,  // If something goes wrong
                                              413,  // responce will contain { "code" : <err_code, "msg" : "<err_msg>" }
                                              422,  // 
                                              501   //
                                              };
    
    /* Definitions */
    Ya_tr::Ya_tr(const std::string& a_k) : api_key{a_k} {
        if (!api_key.size()) {
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
            handle_curl_err("Curl error: " + std::to_string(res));
        }

        if (check_error_code(data)) {
            throw Bad_apikey{err_msg_create(data["message"])};
        }

        avail_lang = get_langs(data["dirs"], lang_delim);
        
        data.clear();
    }

    size_t Ya_tr::handle_data(void *buffer, size_t size, size_t nmemb, void *userp) {
        json data;
        std::istringstream buf { static_cast<char *> (buffer) };
        
        buf >> data;
        
        static_cast<Ya_tr *> (userp)->data = data;
        
        return size * nmemb;
    }
    
    
    std::vector<std::pair<std::string, std::string>> Ya_tr::get_langs(const json::value_type& dirs, const char delim) {
        std::vector<std::pair<std::string, std::string>> langs;
        
        for (auto elem : dirs) {
            std::string from_to { elem.dump() };
            
            delete_quotes(from_to);                
            langs.push_back(split_lang(from_to, delim));
        }
        
        return langs;
    }

    bool Ya_tr::check_error_code(const json::value_type &resp) const {
        if (!resp["code"].is_null() && resp["code"].is_number()) {
            int code = resp["code"];
            
            // Check whether the response contains the "200" code
            return (code == resp_codes[0]) ? false : true;
        }

        return false;
    }
    
    static std::pair<std::string, std::string> split_lang(const std::string& langs, const char delim) {      
        std::string from;
        std::string to;
        
        size_t delim_pos = langs.find(delim);
        
        from = langs.substr(0, delim_pos);
        to = langs.substr(delim_pos + 1);
        
        return std::make_pair(from, to);
    }
    
    static void delete_quotes(std::string& json_str) {      
        json_str.erase(std::remove(json_str.begin(), json_str.end(), json_quotes), json_str.end());
    }
    
    std::string Ya_tr::translate(const std::string& s) {
        std::string key_f = static_cast<std::string> ("key=") + api_key;
        std::string text_f = static_cast<std::string> ("text=") + s;
        std::string lang_f = static_cast<std::string> ("lang=") + from_to.first + "-" + from_to.second;

        std::string whole_f = key_f + "&" + text_f + "&" + lang_f;
        curl_easy_setopt(ya_h, CURLOPT_URL, translate_link.c_str());
        curl_easy_setopt(ya_h, CURLOPT_POSTFIELDS, whole_f.c_str());
        
        CURLcode res = curl_easy_perform(ya_h);
        
        if (res != CURLE_OK) {
            handle_curl_err("Curl error: " + std::string{res});
        }
        
        if (check_error_code(data)) {
            return err_msg_create(data["message"]);
        }
        
        std::string result = data["text"][translate_pos];
        delete_quotes(result);
        
        return result; 
    }
    
    static void handle_curl_err(const std::string& msg) {
        throw std::logic_error(msg);
    }
    
    static std::string err_msg_create(const std::string& err_msg) {
        std::string err = err_msg;
        delete_quotes(err);
        
        return err;
    }
}
