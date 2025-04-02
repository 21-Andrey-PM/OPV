#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

#define ARRAY_SIZE 10000000

void fillArray(vector<int>& array) {
    srand(time(nullptr));
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 100;  
    }
}

long long sequentialSum(const vector<int>& array) {
    long long sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += array[i];
    }
    return sum;
}

long long parallelSum(const vector<int>& array, int num_threads) {
    long long sum = 0;
    omp_set_num_threads(num_threads);  

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += array[i];
    }
    return sum;
}

int main() {
    setlocale(LC_ALL, "ru");
    vector<int> array(ARRAY_SIZE);

    fillArray(array);

    double start_time = omp_get_wtime();
    long long seq_sum = sequentialSum(array);
    double seq_time = omp_get_wtime() - start_time;

    start_time = omp_get_wtime();
    long long par_sum = parallelSum(array, 4);
    double par_time = omp_get_wtime() - start_time;

    cout << "�����: " << seq_sum << ", �����: " << seq_time << " �\n";
    cout << "������� �����: " << par_sum << ", �����: " << par_time << " �\n";

    if (seq_sum == par_sum) {
        cout << "���������\n";
    }
    else {
        cout << "�����������\n";
    }

    return 0;
}
