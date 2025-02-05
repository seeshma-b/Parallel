#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Merging these two halves
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = new int[n1];
    int* R = new int[n2];

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
//will prob need to fix
    int i = 0, j = 0, k = left; 
	while (i < n1 && j < n2) { if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

// recursive merge sort
void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <array_size>" << endl;
        return 1;
    }

    int size = atoi(argv[1]);
    int* arr = new int[size];

    srand(time(0));  // seed for rando number gen
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000;  // random numbers between 0 n 999
    }
// some dude say this is good
    clock_t start = clock();
    mergeSort(arr, 0, size - 1);
    clock_t end = clock();

    double time_taken = double(end - start) / CLOCKS_PER_SEC;
    cout << "Array Size: " << size << " | Time Taken: " << time_taken << " seconds" << endl;
//pls work
    delete[] arr;
    return 0;
}
