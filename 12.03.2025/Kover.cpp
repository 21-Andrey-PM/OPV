#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>

using namespace cv;
using namespace std;

void drawSierpinskiCarpet(Mat& img, int x, int y, int size, int depth) {
    if (depth == 0 || size < 1) {
        return;
    }

    int newSize = size / 3;
    rectangle(img, Point(x + newSize, y + newSize), Point(x + 2 * newSize, y + 2 * newSize), Scalar(203, 192, 255), FILLED);

    if (depth > 1) {
#pragma omp parallel for
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (i == 1 && j == 1) continue;
                drawSierpinskiCarpet(img, x + i * newSize, y + j * newSize, newSize, depth - 1);
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    int size, depth, numThreads;

    cout << "Размер изображения: ";
    cin >> size;
    cout << "Глубина рекурсии: ";
    cin >> depth;
    cout << "Количество потоков: ";
    cin >> numThreads;

    omp_set_num_threads(numThreads);
    Mat img(size, size, CV_8UC3, Scalar(255, 255, 255));

    {
        drawSierpinskiCarpet(img, 0, 0, size, depth);
    }

    imwrite("sierpinski_carpet.png", img);
    imshow("Sierpinski Carpet", img);
    waitKey(0);
    return 0;
}
