#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

const int N = 1000;

void multiply(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

void multiply_parallel(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
#pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

void print_matrix(const vector<vector<int>>& C) {
    for (int i = 0; i < min(N, 10); i++) {
        for (int j = 0; j < min(N, 10); j++) {
            cout << C[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    vector<vector<int>> A(N, vector<int>(N, 1)); 
    vector<vector<int>> B(N, vector<int>(N, 1)); 
    vector<vector<int>> C(N, vector<int>(N, 0));

    double start_serial = omp_get_wtime();
    multiply(A, B, C);
    double end_serial = omp_get_wtime();
    cout << "Однопоточное время выполнения: " << (end_serial - start_serial) << " секунд" << endl;
    cout << "Результат однопоточного умножения:" << endl;
    print_matrix(C);

    double start_parallel = omp_get_wtime();
    multiply_parallel(A, B, C);
    double end_parallel = omp_get_wtime();
    cout << "Параллельное время выполнения: " << (end_parallel - start_parallel) << " секунд" << endl;
    cout << "Результат параллельного умножения:" << endl;
    print_matrix(C);

    return 0;
}
