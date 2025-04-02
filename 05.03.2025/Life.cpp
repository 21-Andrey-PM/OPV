#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <thread>
#include <chrono>
#include <windows.h>

const int WIDTH = 50;
const int HEIGHT = 50;

using namespace std;

void moveCursorToTop() {
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void initialize(vector<vector<int>>& grid) {
    srand(time(0));
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            grid[i][j] = rand() % 2;
        }
    }
}

void printGrid(const vector<vector<int>>& grid) {
    moveCursorToTop();
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            cout << (grid[i][j] ? "O" : " ") << " ";
        }
        cout << endl;
    }
    cout << endl;
}

int countAliveNeighbors(const vector<vector<int>>& grid, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < HEIGHT && ny >= 0 && ny < WIDTH) {
                count += grid[nx][ny];
            }
        }
    }
    return count;
}

void updateGrid(vector<vector<int>>& grid) {
    vector<vector<int>> newGrid = grid; 
    auto start = chrono::high_resolution_clock::now();

    #pragma omp parallel for
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            int aliveNeighbors = countAliveNeighbors(grid, i, j);
            if (grid[i][j] == 1) {
                if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                    newGrid[i][j] = 0;
                }
            }
            else {
                if (aliveNeighbors == 3) {
                    newGrid[i][j] = 1;
                }
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end - start;
    cout << "Время обновления состояния: " << elapsed.count() << " ms" << endl;

    grid = newGrid;
}

int countAliveCells(const vector<vector<int>>& grid) {
    int aliveCount = 0;
    #pragma omp parallel for reduction(+:aliveCount)
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            aliveCount += grid[i][j];
        }
    }
    return aliveCount;
}

int main() {
    setlocale(LC_ALL, "ru");
    vector<vector<int>> grid(HEIGHT, vector<int>(WIDTH, 0));
    initialize(grid);

    int iterations, numThreads;
    cout << "Введите количество итераций: ";
    cin >> iterations;
    cout << "Введите число потоков: ";
    cin >> numThreads;

    omp_set_num_threads(numThreads);

    for (int i = 0; i < iterations; ++i) {
        printGrid(grid);
        int aliveCells = countAliveCells(grid);
        cout << "Итерация " << i + 1 << endl;
        updateGrid(grid);
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    return 0;
}