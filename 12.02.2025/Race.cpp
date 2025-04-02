#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

int shared_resource = 0;

mutex mtx;

void access_shared_resource(int id) {
    lock_guard<mutex> lock(mtx);

    cout << "����� " << id << " ����� ������ � ��������." << endl;
    shared_resource++;

    this_thread::sleep_for(chrono::milliseconds(100));

    cout << "����� " << id << " �������� ������ � ��������. ������: " << shared_resource << endl;
}

int main() {
    setlocale(LC_ALL, "ru");
    const int num_threads = 8;

    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(thread(access_shared_resource, i + 1));
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "�������� �������� �������: " << shared_resource << endl;

    return 0;
}
