#include "blocking_queue.hpp"
#include "rapidjson/document.h"
#include <unordered_set>
#include <iostream>
#include <thread>
#include <mutex>
#include <curl/curl.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

const string BASE_URL = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";
bool debug = false;
int MAX_THREADS = 8;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string fetch_neighbors(CURL* curl, const string& node) {
    string url = BASE_URL + curl_easy_escape(curl, node.c_str(), node.length());
    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_perform(curl);
    return response;
}

vector<string> parse_neighbors(const string& json) {
    vector<string> neighbors;
    rapidjson::Document d;
    d.Parse(json.c_str());
    if (d.HasMember("neighbors") && d["neighbors"].IsArray()) {
        for (auto& n : d["neighbors"].GetArray())
            neighbors.push_back(n.GetString());
    }
    return neighbors;
}

void worker(BlockingQueue<pair<string, int>>& queue, unordered_set<string>& visited, mutex& visited_mutex,
            mutex& output_mutex, vector<string>& output, int max_depth, CURL* curl) {
    while (auto item = queue.pop()) {
        auto [node, depth] = *item;
        if (depth > max_depth) continue;

        {
            lock_guard<mutex> lock(output_mutex);
            output.push_back(node);
        }

        if (depth == max_depth) continue;

        string res = fetch_neighbors(curl, node);
        for (const auto& neighbor : parse_neighbors(res)) {
            lock_guard<mutex> lock(visited_mutex);
            if (visited.insert(neighbor).second) {
                queue.push({neighbor, depth + 1});
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./crawler <start_node> <depth>" << endl;
        return 1;
    }

    string start_node = argv[1];
    int max_depth = stoi(argv[2]);

    CURL* curl = curl_easy_init();
    BlockingQueue<pair<string, int>> queue;
    unordered_set<string> visited;
    vector<string> output;
    mutex visited_mutex, output_mutex;

    visited.insert(start_node);
    queue.push({start_node, 0});

    auto start = chrono::steady_clock::now();
    vector<thread> threads;

    for (int i = 0; i < MAX_THREADS; ++i)
        threads.emplace_back(worker, ref(queue), ref(visited), ref(visited_mutex), ref(output_mutex), ref(output), max_depth, curl);

    for (auto& t : threads) t.join();
    auto end = chrono::steady_clock::now();
    queue.done();
    curl_easy_cleanup(curl);

    for (const auto& node : output) cout << "- " << node << endl;

    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time: " << elapsed.count() << "s\n";

    ofstream log("output/tomhanks_depth" + to_string(max_depth) + ".txt");
    for (const auto& node : output) log << node << "\n";
    log << "Elapsed time: " << elapsed.count() << "s\n";
}