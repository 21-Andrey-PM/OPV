#include <iostream>
#include <cmath>
#include <omp.h>


using namespace std;

#define N 1000000000
#define A 0        
#define PI acos(-1)

double f(double x) {
    return sin(x);
}

double sequentialIntegration(double a, double b, int n) {
    if (n <= 0) return 0;
    double h = (b - a) / n;
    double sum = 0;

    for (int i = 0; i < n; i++) {
        double x = a + i * h + h / 2;
        sum += f(x);
    }

    return sum * h;
}

double parallelIntegration(double a, double b, int n, int num_threads) {
    if (n <= 0) return 0;
    double h = (b - a) / n;
    double sum = 0;

    omp_set_num_threads(num_threads);

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        double x = a + i * h + h / 2;
        sum += f(x);
    }

    return sum * h;
}

int main() {
    setlocale(LC_ALL, "ru");
    double exact_value = -cos(PI) + cos(A);

    double start_time = omp_get_wtime();
    double seq_result = sequentialIntegration(A, PI, N);
    double seq_time = omp_get_wtime() - start_time;

    start_time = omp_get_wtime();
    double par_result = parallelIntegration(A, PI, N, 4);
    double par_time = omp_get_wtime() - start_time;

    cout << "Ручками: " << exact_value << endl;
    cout << "Результат: " << seq_result << ", Время: " << seq_time << " с\n";
    cout << "Паралел результат: " << par_result << ", Время: " << par_time << " с\n";
    return 0;
}
