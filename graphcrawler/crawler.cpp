#include <iostream>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <queue>
#include <unordered_set>
#include <chrono>

// encode spaces as %20 for URL encoding bc hardcoding didnt work??
std::string encodeSpaces(const std::string& input) {
    std::string encoded = input;
    std::string::size_type pos = 0;
    while ((pos = encoded.find(" ", pos)) != std::string::npos) {
        encoded.replace(pos, 1, "%20");
        pos += 3;
    }
    return encoded;
}

// storing API output
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// fetch neighbors from API
std::vector<std::string> fetchNeighbors(const std::string& node) {
    std::string encodedNode = encodeSpaces(node);
    std::string url = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/" + encodedNode;

    CURL* curl = curl_easy_init();
    std::vector<std::string> neighbors;

    if (curl) {
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        } else {
            // parse JSON outputs
            rapidjson::Document doc;
            doc.Parse(response.c_str());

            if (!doc.HasParseError() && doc.HasMember("neighbors") && doc["neighbors"].IsArray()) {
                for (const auto& neighbor : doc["neighbors"].GetArray()) {
                    neighbors.push_back(neighbor.GetString());
                }
            } else {
                std::cerr << "Errors: Invalid JSON response" << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }
    return neighbors;
}

// BFS
void BFS_Crawler(const std::string& startNode, int depthLimit) {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::queue<std::pair<std::string, int>> q;
    std::unordered_set<std::string> visited;

    q.push({startNode, 0});
    visited.insert(startNode);

    while (!q.empty()) {
        auto [currentNode, depth] = q.front();
        q.pop();

        std::cout << "Visiting: " << currentNode << " (Depths: " << depth << ")" << std::endl;

        if (depth < depthLimit) {
            std::vector<std::string> neighbors = fetchNeighbors(currentNode);
            for (const auto& neighbor : neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push({neighbor, depth + 1}); 
                }
            }
        }        
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    std::cout << "Execution Time: " << duration.count() << " seconds\n";
}

// Main
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./crawler <start_node> <depth_limit>\n";
        return 1;
    }

    std::string startNode = argv[1];
    int depthLimit = std::stoi(argv[2]);

    BFS_Crawler(startNode, depthLimit);
    return 0;
}
