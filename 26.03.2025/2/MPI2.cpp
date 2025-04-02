#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int N = 1000; 

using namespace std;

void generate_matrix(vector<int>& matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = rand() % 10;
    }
}

void print_sample(const vector<int>& matrix, int rows, int cols) {
    int display_size = min(10, rows);
    for (int i = 0; i < display_size; i++) {
        for (int j = 0; j < display_size; j++) {
            cout << matrix[i * cols + j] << " ";
        }
        cout << endl;
    }
    cout << "..." << endl;
}


int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = N / size;
    vector<int> A, B(N * N), C;
    vector<int> local_A(rows_per_proc * N);
    vector<int> local_C(rows_per_proc * N, 0);

    if (rank == 0) {
        A.resize(N * N);
        C.resize(N * N);
        srand(time(nullptr));
        generate_matrix(A, N, N);
        generate_matrix(B, N, N);
    }

    MPI_Bcast(B.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(A.data(), rows_per_proc * N, MPI_INT, local_A.data(), rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                local_C[i * N + j] += local_A[i * N + k] * B[k * N + j];
            }
        }
    }
    double end_time = MPI_Wtime();

    MPI_Gather(local_C.data(), rows_per_proc * N, MPI_INT, C.data(), rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Time: " << (end_time - start_time) << " sec" << endl;
        cout << "Few elements:" << endl;
        print_sample(C, N, N);
    }

    MPI_Finalize();
    return 0;
}
