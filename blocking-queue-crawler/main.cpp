#include "blocking_queue.hpp"
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>

using namespace std;
using namespace rapidjson;

const string BASE_URL = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";
const int MAX_THREADS = 4;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

string fetch_neighbors(const string& node) {
    CURL* curl = curl_easy_init();
    string response;
    if (!curl) return response;

    string url = BASE_URL + curl_easy_escape(curl, node.c_str(), node.length());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "ParallelCrawler/1.0");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res == CURLE_OK ? response : "{}";
}

vector<string> parse_neighbors(const string& json_str) {
    vector<string> neighbors;
    Document doc;
    doc.Parse(json_str.c_str());
    if (!doc.IsObject() || !doc.HasMember("neighbors") || !doc["neighbors"].IsArray())
        return neighbors;
    for (const auto& n : doc["neighbors"].GetArray())
        if (n.IsString()) neighbors.push_back(n.GetString());
    return neighbors;
}

void worker(BlockingQueue<pair<string, int>>& queue,
            unordered_set<string>& visited,
            mutex& visited_mutex,
            mutex& output_mutex,
            vector<string>& output,
            int max_depth) {
    while (auto item = queue.pop()) {
        auto [node, depth] = *item;
        if (depth > max_depth) continue;

        {
            lock_guard<mutex> lock(output_mutex);
            output.push_back(node);
        }

        if (depth == max_depth) continue;
        vector<string> neighbors = parse_neighbors(fetch_neighbors(node));
        for (const auto& neighbor : neighbors) {
            lock_guard<mutex> lock(visited_mutex);
            if (visited.insert(neighbor).second)
                queue.push({neighbor, depth + 1});
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./crawler \"<start_node>\" <depth>\n";
        return 1;
    }

    string start_node = argv[1];
    int depth = stoi(argv[2]);

    BlockingQueue<pair<string, int>> queue;
    unordered_set<string> visited;
    vector<string> output;
    mutex visited_mutex, output_mutex;

    visited.insert(start_node);
    queue.push({start_node, 0});

    auto t_start = chrono::steady_clock::now();

    vector<thread> threads;
    for (int i = 0; i < MAX_THREADS; ++i)
        threads.emplace_back(worker, ref(queue), ref(visited),
                             ref(visited_mutex), ref(output_mutex), ref(output), depth);

    queue.done();
    for (auto& t : threads) t.join();

    auto t_end = chrono::steady_clock::now();
    chrono::duration<double> elapsed = t_end - t_start;

    for (const auto& node : output)
        cout << "- " << node << endl;
    cout << "Time to crawl: " << elapsed.count() << "s\n";

    ofstream file("output/tomhanks_depth" + to_string(depth) + ".txt");
    for (const auto& node : output) file << node << "\n";
    file << "Time: " << elapsed.count() << "s\n";
}
