#include <iostream>
#include <curl/curl.h>
#include <rapidjson/document.h>  // RapidJSONs
#include <string>
#include <algorithm>

std::string encodeSpaces(const std::string& input) {
    std::string encoded = input;
    return encoded;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// fetch and parse JSON 
void fetchAndParseJSON(const std::string& node) {
    std::string encodedNode = encodeSpaces(node);
    std::string url = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/" + encodedNode;

    std::cout << "Fetching: " << url << std::endl;
    
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL Errors: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse json using RapidJSON
            rapidjson::Document doc;
            doc.Parse(response.c_str());

            // if parsing was successful and neighbors exists
            if (!doc.HasParseError() && doc.HasMember("neighbors") && doc["neighbors"].IsArray()) {
                std::cout << "Node: " << doc["node"].GetString() << "\nNeighbors:\n";
                for (const auto& neighbor : doc["neighbors"].GetArray()) {
                    std::cout << "- " << neighbor.GetString() << std::endl;
                }
            } else {
                std::cerr << "Error: Invalid JSON response" << std::endl;
            }
        }
        
        curl_easy_cleanup(curl);
    }
}

int main() {
    std::string node = "Tom%20Hanks";  // the tomhanks output. REMEMBER %20 for spaces bc unicode ew
    fetchAndParseJSON(node);
    return 0;
}