#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <cstdio>
#include <cstdlib>
#include <curl/curl.h>
#include <stdexcept>
#include "rapidjson/error/error.h"
#include "rapidjson/reader.h"
#include <rapidjson/document.h>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;
using namespace rapidjson;

bool debug = false;
const string SERVICE_URL = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";

struct ParseException : std::runtime_error, rapidjson::ParseResult {
    ParseException(rapidjson::ParseErrorCode code, const char* msg, size_t offset) : 
        std::runtime_error(msg), 
        rapidjson::ParseResult(code, offset) {}
};

#ifndef RAPIDJSON_PARSE_ERROR_NORETURN
#define RAPIDJSON_PARSE_ERROR_NORETURN(code, offset) \
    throw ParseException(code, #code, offset)
#endif

string url_encode(CURL* curl, string input) {
    char* out = curl_easy_escape(curl, input.c_str(), input.size());
    string s = out;
    curl_free(out);
    return s;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

string fetch_neighbors(CURL* curl, const string& node) {
    string url = SERVICE_URL + url_encode(curl, node);
    string response;

    if (debug)
        cout << "Sending request to: " << url << endl;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "User-Agent: C++-Client/1.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cerr << "CURL error: " << curl_easy_strerror(res) << endl;
    } else if (debug) {
        cout << "CURL request successful!" << endl;
    }

    curl_slist_free_all(headers);
    if (debug)
        cout << "Response received: " << response << endl;

    return (res == CURLE_OK) ? response : "{}";
}
vector<string> get_neighbors(const string& json_str) {
    vector<string> neighbors;
    try {
        Document doc;
        doc.Parse(json_str.c_str());

        if (doc.HasMember("neighbors") && doc["neighbors"].IsArray()) {
            for (const auto& neighbor : doc["neighbors"].GetArray())
                neighbors.push_back(neighbor.GetString());
        }
    } catch (const ParseException& e) {
        cerr << "Error while parsing JSON: " << json_str << endl;
        throw e;
    }
    return neighbors;
}
vector<vector<string>> bfs(CURL* curl, const string& start, int depth) {
    vector<vector<string>> levels;
    unordered_set<string> visited;
    mutex mtx;
    const int MAX_THREADS = 8;

    levels.push_back({start});
    visited.insert(start);

    for (int d = 0; d < depth; d++) {
        if (debug)
            cout << "starting level: " << d << "\n";

        levels.push_back({});
        vector<thread> threads;
        int num_nodes = levels[d].size();
        int num_threads = min(MAX_THREADS, num_nodes);
        int chunk_size = (num_nodes + num_threads - 1) / num_threads;
	    
        for (int t = 0; t < num_threads; ++t) {
            int start_idx = t * chunk_size;
            int end_idx = min((t + 1) * chunk_size, num_nodes);

            threads.emplace_back([&, start_idx, end_idx]() {
                for (int i = start_idx; i < end_idx; ++i) {
                    const string& node = levels[d][i];
                    try {
                        if (debug)
                            cout << "Thread " << t << " expanding: " << node << "\n";

                        for (const auto& neighbor : get_neighbors(fetch_neighbors(curl, node))) {
                            lock_guard<mutex> lock(mtx);
                            if (visited.insert(neighbor).second) {
                                levels[d + 1].push_back(neighbor);
                            }
                        }
                    } catch (const ParseException& e) {
                        cerr << "Error fetching neighbors of: " << node << endl;
                    }
                }
            });
        }
        for (auto& t : threads) t.join();
    }
    return levels;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <node_name> <depth>\n";
        return 1;
    }
    string start_node = argv[1];
    int depth;
    try {
        depth = stoi(argv[2]);
    } catch (const exception& e) {
        cerr << "Error: Depth must be an integer.\n";
        return 1;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL" << endl;
        return -1;
    }

    const auto start_time = chrono::steady_clock::now();
    auto levels = bfs(curl, start_node, depth);

    for (const auto& level : levels) {
        for (const auto& node : level)
            cout << "- " << node << "\n";
        cout << level.size() << "\n";
    }

    const auto end_time = chrono::steady_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;
    cout << "Time to crawl: " << elapsed.count() << "s\n";

    curl_easy_cleanup(curl);
    return 0;
}
