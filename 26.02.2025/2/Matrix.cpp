#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

#define N 10000 
#define MN 20000

using namespace std;

void multiply_matrix_vector(const vector<vector<double>>& matrix, const vector<double>& vec, vector<double>& result) {
    for (int i = 0; i < N; i++) {
        result[i] = 0;
        for (int j = 0; j < MN; j++) {
            result[i] += matrix[i][j] * vec[j];
        }
    }
}

void parallel_multiply_matrix_vector(const vector<vector<double>>& matrix, const vector<double>& vec, vector<double>& result) {
#pragma omp parallel for
    for (int i = 0; i < N; i++) {
        result[i] = 0;
        for (int j = 0; j < MN; j++) {
            result[i] += matrix[i][j] * vec[j];
        }
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    vector<vector<double>> matrix(N, vector<double>(MN));
    vector<double> vec(MN);
    vector<double> result_seq(N);
    vector<double> result_par(N);

    srand(time(nullptr));
    for (int i = 0; i < N; i++) {
        vec[i] = rand() % 10;
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10;
        }
    }

    double start_time = omp_get_wtime();
    multiply_matrix_vector(matrix, vec, result_seq);
    double end_time = omp_get_wtime();
    cout << "Время последовательное: " << (end_time - start_time) << " секунд" << endl;

    start_time = omp_get_wtime();
    parallel_multiply_matrix_vector(matrix, vec, result_par);
    end_time = omp_get_wtime();
    cout << "Время параллельное: " << (end_time - start_time) << " секунд" << endl;

    return 0;
}
