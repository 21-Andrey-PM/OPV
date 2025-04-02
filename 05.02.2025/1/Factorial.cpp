#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

unsigned long long sequential_factorial(int n) {
    if (n < 0) return 0;
    unsigned long long result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

unsigned long long parallel_result = 1;
mutex result_mutex;

void partial_factorial(int start, int end) {
    unsigned long long partial = 1;
    for (int i = start; i <= end; ++i) {
        partial *= i;
    }

    lock_guard<mutex> lock(result_mutex);
    parallel_result *= partial;
}

unsigned long long parallel_factorial(int n, int num_threads = 4) {
    if (n < 0) return 0;
    if (n < 2) return 1;

    parallel_result = 1; 

    int elements_per_thread = n / num_threads;
    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        int start = i * elements_per_thread + 1;
        int end = (i == num_threads - 1) ? n : (i + 1) * elements_per_thread;

        threads.emplace_back(partial_factorial, start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return parallel_result;
}

int main() {
    int number;
    cout << "Enter a number: ";
    cin >> number;

    auto start_seq = chrono::high_resolution_clock::now();
    unsigned long long seq_result = sequential_factorial(number);
    auto end_seq = chrono::high_resolution_clock::now();
    chrono::duration<double> seq_duration = end_seq - start_seq;

    auto start_par = chrono::high_resolution_clock::now();
    unsigned long long par_result = parallel_factorial(number);
    auto end_par = chrono::high_resolution_clock::now();
    chrono::duration<double> par_duration = end_par - start_par;

    cout << "\nSequential:" << endl;
    cout << "Factorial  " << number << " = " << seq_result << endl;
    cout << "Time: " << seq_duration.count() << " sec" << endl;

    cout << "\nParallel:" << endl;
    cout << "Factorial " << number << " = " << par_result << endl;
    cout << "Time: " << par_duration.count() << " sec" << endl;

    return 0;
}