#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <complex>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;
using namespace cv;

const int WIDTH = 800;
const int HEIGHT = 800;
const int MAX_ITER = 1000;

const double X_MIN = -2.5;
const double X_MAX = 1.0;
const double Y_MIN = -1.5;
const double Y_MAX = 1.5;

float mandelbrot(double x0, double y0) {
    complex<double> z(0.0, 0.0);
    complex<double> c(x0, y0);
    int iter = 0;
    while (abs(z) < 2.0 && iter < MAX_ITER) {
        z = z * z + c;
        iter++;
    }
    return iter;
}

Vec3b getColor(float t) {
    t = t / MAX_ITER;
    uchar r = (uchar)(255.0 * pow(t, 0.3));
    uchar g = (uchar)(255.0 * pow(t, 0.5));
    uchar b = (uchar)(255.0 * pow(t, 0.7));
    return Vec3b(b, g, r);
}

void generateFractal(Mat& image, int start_row, int total_rows) {
    for (int y = 0; y < total_rows; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double real = X_MIN + x * (X_MAX - X_MIN) / WIDTH;
            double imag = Y_MIN + (start_row + y) * (Y_MAX - Y_MIN) / HEIGHT;

            float iter = mandelbrot(real, imag);

            Vec3b color;
            if (iter >= MAX_ITER) {
                color = Vec3b(0, 0, 0);
            }
            else {
                color = getColor(iter);
            }

            image.at<Vec3b>(y, x) = color;
        }
    }
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru");
    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        auto start = chrono::high_resolution_clock::now();

        Mat serial_image(HEIGHT, WIDTH, CV_8UC3);
        generateFractal(serial_image, 0, HEIGHT);

        auto end = chrono::high_resolution_clock::now();
        double duration = chrono::duration<double>(end - start).count();

        cout << "Posled time: " << duration << " sec" << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD); 
    int base_rows = HEIGHT / size;
    int extra_rows = HEIGHT % size;

    int start_row = base_rows * rank;
    if (rank < extra_rows) {
        start_row += rank;
    }
    else {
        start_row += extra_rows;
    }

    int local_height = base_rows;
    if (rank < extra_rows) {
        local_height += 1;
    }

    Mat local_image(local_height, WIDTH, CV_8UC3);

    auto start_mpi = chrono::high_resolution_clock::now();

    generateFractal(local_image, start_row, local_height);

    if (rank == 0) {
        Mat full_image(HEIGHT, WIDTH, CV_8UC3);
        local_image.copyTo(full_image(Rect(0, start_row, WIDTH, local_height)));

        for (int i = 1; i < size; ++i) {
            int other_start_row = base_rows * i;
            if (i < extra_rows) {
                other_start_row += i;
            }
            else {
                other_start_row += extra_rows;
            }

            int other_rows = base_rows;
            if (i < extra_rows) {
                other_rows += 1;
            }

            int recv_size = other_rows * WIDTH * 3;
            vector<uchar> buffer(recv_size);

            MPI_Recv(buffer.data(), recv_size, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            Mat temp(other_rows, WIDTH, CV_8UC3, buffer.data());
            temp.copyTo(full_image(Rect(0, other_start_row, WIDTH, other_rows)));
        }

        auto end_mpi = chrono::high_resolution_clock::now();
        double duration = chrono::duration<double>(end_mpi - start_mpi).count();
        cout << "MPI time: " << duration << " sec" << endl;

        imwrite("mandelbrot_mpi.png", full_image);
        imshow("Mandelbrot MPI", full_image);
        waitKey(0);

    }
    else {
        int send_size = local_height * WIDTH * 3;
        MPI_Send(local_image.data, send_size, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
