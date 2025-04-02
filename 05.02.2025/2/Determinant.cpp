#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>  

using namespace std;

mutex mtx;

vector<vector<int>> getCofactor(const vector<vector<int>>& matrix, int row, int col) {
    int n = matrix.size();
    vector<vector<int>> cofactor;
    for (int i = 0; i < n; ++i) {
        if (i == row) continue;
        vector<int> cofactorRow;
        for (int j = 0; j < n; ++j) {
            if (j == col) continue;
            cofactorRow.push_back(matrix[i][j]);
        }
        cofactor.push_back(cofactorRow);
    }
    return cofactor;
}

int calculateDeterminant(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    if (n == 1) return matrix[0][0];
    if (n == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

    int det = 0;
    vector<thread> threads;
    vector<int> results(n, 0);

    auto computeCofactor = [&](int col) {
        vector<vector<int>> cofactor = getCofactor(matrix, 0, col);
        int sign = (col % 2 == 0) ? 1 : -1;
        int cofactorDet = calculateDeterminant(cofactor);
        mtx.lock();
        results[col] = sign * matrix[0][col] * cofactorDet;
        mtx.unlock();
        };

    for (int col = 0; col < n; ++col) {
        threads.emplace_back(computeCofactor, col);
    }

    for (auto& th : threads) {
        th.join();
    }

    for (int res : results) {
        det += res;
    }

    return det;
}

int main() {
    int n;
    cout << "Enter the size: ";
    cin >> n;

    vector<vector<int>> matrix(n, vector<int>(n));

    ifstream file("matrix.txt");

    if (!file) {
        cerr << "Error opening" << endl;
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!(file >> matrix[i][j])) {
                cerr << "Error reading" << endl;
                return 1;
            }
        }
    }

    file.close();

    int det = calculateDeterminant(matrix);
    cout << "Determinant = " << det << endl;

    return 0;
}
