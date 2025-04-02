#include <iostream>
#include <omp.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

void oddEvenSort(vector<int>& arr) {
    int n = arr.size();
    bool sorted = false;

    while (!sorted) {
        sorted = true;

#pragma omp parallel for shared(arr, sorted)
        for (int i = 0; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

#pragma omp barrier

#pragma omp parallel for shared(arr, sorted)
        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

#pragma omp barrier
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    srand(time(nullptr));
    int n = 50;
    vector<int> arr(n);

    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
    }

    cout << "Исходный массив:" << endl;
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    oddEvenSort(arr);

    cout << "Отсортированный массив:" << endl;
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    return 0;
}
