#ifndef BLOCKING_QUEUE_HPP
#define BLOCKING_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class BlockingQueue {
    std::queue<T> q;
    std::mutex m;
    std::condition_variable cv;
    bool finished = false;

public:
    void push(const T& item) {
        {
            std::lock_guard<std::mutex> lock(m);
            q.push(item);
        }
        cv.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&]{ return !q.empty() || finished; });
        if (q.empty()) return std::nullopt;
        T item = q.front();
        q.pop();
        return item;
    }

    void done() {
        {
            std::lock_guard<std::mutex> lock(m);
            finished = true;
        }
        cv.notify_all();
    }
};

#endif
