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
#include <atomic>
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
            int max_depth,
            atomic<int>& active_threads) {
    while (true) {
        auto item = queue.pop();
        if (!item.has_value()) break;

        active_threads++;
        auto [node, depth] = *item;

        {
            lock_guard<mutex> lock(output_mutex);
            output.push_back(node);
        }

        if (depth < max_depth) {
            string json = fetch_neighbors(node);
            vector<string> neighbors = parse_neighbors(json);

            for (const auto& neighbor : neighbors) {
                lock_guard<mutex> lock(visited_mutex);
                if (visited.insert(neighbor).second) {
                    queue.push({neighbor, depth + 1});
                }
            }
        }

        active_threads--;
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
    atomic<int> active_threads(0);

    visited.insert(start_node);
    queue.push({start_node, 0});

    auto start = chrono::steady_clock::now();

    vector<thread> threads;
    for (int i = 0; i < MAX_THREADS; ++i) {
        threads.emplace_back(worker, ref(queue), ref(visited), ref(visited_mutex),
                             ref(output_mutex), ref(output), depth, ref(active_threads));
    }

    // Wait for queue to drain and no threads to be active
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(100));
        if (queue.empty() && active_threads.load() == 0) {
            queue.done();
            break;
        }
    }

    for (auto& t : threads) t.join();

    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed = end - start;

    for (const auto& node : output)
        cout << "- " << node << endl;
    cout << "Time to crawl: " << elapsed.count() << "s\n";

    ofstream file("output/" + start_node + "_depth" + to_string(depth) + ".txt");
    for (const auto& node : output) file << node << "\n";
    file << "Time: " << elapsed.count() << "s\n";

    return 0;
}
