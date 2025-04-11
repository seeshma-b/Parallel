#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>

#define DEBUG 0

const size_t PARALLEL_THRESHOLD = 50000;

void generateMergeSortData(std::vector<int>& arr, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        arr[i] = rand();
    }
}

void checkMergeSortResult(std::vector<int>& arr, size_t n) {
    bool ok = true;
    for (size_t i = 1; i < n; ++i)
        if (arr[i] < arr[i - 1])
            ok = false;
    if (!ok)
        std::cerr << "notok" << std::endl;
}

void merge(int* arr, size_t l, size_t mid, size_t r, int* temp) {
    if (l == r) return;
    if (r - l == 1) {
        if (arr[l] > arr[r]) {
            std::swap(arr[l], arr[r]);
        }
        return;
    }

    size_t i, j, k;
    size_t n = mid - l;
    for (i = 0; i < n; ++i)
        temp[i] = arr[l + i];

    i = 0;
    j = mid;
    k = l;

    while (i < n && j <= r) {
        if (temp[i] <= arr[j]) {
            arr[k++] = temp[i++];
        } else {
            arr[k++] = arr[j++];
        }
    }

    while (i < n) {
        arr[k++] = temp[i++];
    }
}

void mergesort_seq(int* arr, size_t l, size_t r, int* temp) {
    if (l < r) {
        size_t mid = (l + r) / 2;
        mergesort_seq(arr, l, mid, temp);
        mergesort_seq(arr, mid + 1, r, temp);
        merge(arr, l, mid + 1, r, temp);
    }
}

void mergesort_par(int* arr, size_t l, size_t r, int* temp) {
    if (l < r) {
        if (r - l + 1 <= PARALLEL_THRESHOLD) {
            mergesort_seq(arr, l, r, temp);
            return;
        }

        size_t mid = (l + r) / 2;
        
        std::thread left_thread(mergesort_par, arr, l, mid, temp);
        mergesort_par(arr, mid + 1, r, temp);
        left_thread.join();

        merge(arr, l, mid + 1, r, temp);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <n>" << std::endl;
        return -1;
    }

    size_t n = atol(argv[1]);
    std::vector<int> arr(n);
    generateMergeSortData(arr, n);

#if DEBUG
    for (size_t i = 0; i < n; ++i)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
#endif

    auto start = std::chrono::system_clock::now();

    std::vector<int> temp(n);
    mergesort_par(&(arr[0]), 0, n - 1, &(temp[0]));

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cerr << elapsed_seconds.count() << std::endl;

    checkMergeSortResult(arr, n);

#if DEBUG
    for (size_t i = 0; i < n; ++i)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
#endif

    return 0;
}
