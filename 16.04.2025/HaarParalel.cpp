#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

using namespace cv;
using namespace std;

queue<Mat> frameQueue;
queue<Mat> processedQueue;
mutex frameMutex, processedMutex;
condition_variable frameCondVar, processedCondVar;
atomic<bool> finishedReading(false);
atomic<bool> finishedProcessing(false);

CascadeClassifier face_cascade, eyes_cascade, smile_cascade;

void readFrames(VideoCapture& cap) {
    Mat frame;
    while (cap.read(frame)) {
        unique_lock<mutex> lock(frameMutex);
        frameQueue.push(frame.clone());
        frameCondVar.notify_one();
    }
    finishedReading = true;
    frameCondVar.notify_all();
}

void processFrames() {
    while (!finishedReading || !frameQueue.empty()) {
        Mat frame;
        {
            unique_lock<mutex> lock(frameMutex);
            frameCondVar.wait(lock, [] { return !frameQueue.empty() || finishedReading; });
            if (frameQueue.empty()) continue;
            frame = frameQueue.front();
            frameQueue.pop();
        }

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        vector<Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.3, 5);

        for (const auto& face : faces) {
            rectangle(frame, face, Scalar(255, 0, 0), 2);
            Mat faceROI = gray(face);
            vector<Rect> eyes;
            eyes_cascade.detectMultiScale(faceROI, eyes, 1.08, 20, 0, Size(55, 50));
            for (const auto& eye : eyes) {
                Rect eyeRect(eye.x + face.x, eye.y + face.y, eye.width, eye.height);
                rectangle(frame, eyeRect, Scalar(0, 255, 0), 2);
            }

            vector<Rect> smiles;
            smile_cascade.detectMultiScale(faceROI, smiles, 1.2, 50);
            for (const auto& smile : smiles) {
                Rect smileRect(smile.x + face.x, smile.y + face.y, smile.width, smile.height);
                rectangle(frame, smileRect, Scalar(0, 255, 255), 2);
            }
        }

        {
            unique_lock<mutex> lock(processedMutex);
            processedQueue.push(frame);
            processedCondVar.notify_one();
        }
    }

    finishedProcessing = true;
    processedCondVar.notify_all();
}

void saveFrames(VideoWriter& writer) {
    while (!finishedProcessing || !processedQueue.empty()) {
        Mat frame;
        {
            unique_lock<mutex> lock(processedMutex);
            processedCondVar.wait(lock, [] { return !processedQueue.empty() || finishedProcessing; });
            if (processedQueue.empty()) continue;
            frame = processedQueue.front();
            processedQueue.pop();
        }

        writer.write(frame);
        imshow("Распознавание", frame);
        if (waitKey(1) == 27) break;
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    using namespace chrono;
    auto start_time = high_resolution_clock::now();

    if (!face_cascade.load("haarcascade_frontalface_default.xml") ||
        !eyes_cascade.load("haarcascade_eye.xml") ||
        !smile_cascade.load("haarcascade_smile.xml")) {
        cout << "Ошибка загрузки каскадов" << endl;
        return -1;
    }

    VideoCapture cap("ZUA.mp4");
    if (!cap.isOpened()) {
        cout << "Не удалось открыть видео" << endl;
        return -1;
    }

    int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);

    VideoWriter video("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frame_width, frame_height));

    if (!video.isOpened()) {
        cout << "Не удалось создать видеофайл" << endl;
        return -1;
    }

    thread reader(readFrames, ref(cap));
    thread processor(processFrames);
    thread writer(saveFrames, ref(video));

    reader.join();
    processor.join();
    writer.join();

    cap.release();
    video.release();
    destroyAllWindows();

    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end_time - start_time);

    cout << "Время выполнения: " << duration.count() << " секунд" << endl;

    return 0;
}
