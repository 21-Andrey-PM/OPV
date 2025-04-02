#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#define ARRAY_SIZE 10000000

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<int> numbers;
    int local_size = ARRAY_SIZE / size;
    vector<int> local_numbers(local_size);

    double start_time, end_time;
    if (rank == 0) {
        numbers.resize(ARRAY_SIZE);
        srand(time(nullptr));
        for (int i = 0; i < ARRAY_SIZE; ++i) {
            numbers[i] = rand() % 100;
        }

        double seq_start = MPI_Wtime();
        long long sequential_sum = 0;
        for (int num : numbers) {
            sequential_sum += num;
        }
        double seq_end = MPI_Wtime();
        cout << "Seq sum: " << sequential_sum << " (Time: " << (seq_end - seq_start) << " sec)\n";
    }

    start_time = MPI_Wtime();

    MPI_Scatter(numbers.data(), local_size, MPI_INT, local_numbers.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

    int local_sum = 0;
    for (int num : local_numbers) {
        local_sum += num;
    }

    int global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    if (rank == 0) {
        cout << "Global sum: " << global_sum << " (Time: " << (end_time - start_time) << " sec)\n";
    }

    MPI_Finalize();
    return 0;
}