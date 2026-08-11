#include <iostream>
#include <fstream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/geometry/2d.hpp>

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
    return std::string(buffer);
}

int main(int argc, char** argv) {
    std::ofstream logFile("motion_log.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл для логов!" << std::endl;
        return -1;
    }

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось подключиться к камере." << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    cv::namedWindow("Motion Detector (q to quit)", cv::WINDOW_NORMAL | cv::WINDOW_GUI_NORMAL);
    cv::resizeWindow("Motion Detector (q to quit)", 1280, 720);

    cv::Mat prevFrame, currentFrame, gray, diff, thresh;
    
    std::cout << "Система обнаружения движения запущена." << std::endl;
    std::cout << "Нажмите английскую 'q' в окне с видео для выхода." << std::endl;

    while (true) {
        cap >> currentFrame;
        if (currentFrame.empty()) {
            std::cerr << "Камера не передала кадр, выходим..." << std::endl;
            break;
        }

        cv::cvtColor(currentFrame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);

        if (prevFrame.empty()) {
            gray.copyTo(prevFrame);
            continue;
        }

        cv::absdiff(prevFrame, gray, diff);
        cv::threshold(diff, thresh, 25, 255, cv::THRESH_BINARY);
        cv::dilate(thresh, thresh, cv::Mat(), cv::Point(-1, -1), 2);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        bool motionDetected = false;

        for (size_t i = 0; i < contours.size(); i++) {
            if (cv::contourArea(contours[i]) < 1500) {
                continue;
            }

            cv::Rect rect = cv::boundingRect(contours[i]);
            cv::rectangle(currentFrame, rect, cv::Scalar(0, 255, 0), 2);
            motionDetected = true;
        }

        if (motionDetected) {
            std::string timeStr = getCurrentTime();
            logFile << "[" << timeStr << "] Обнаружено движение в кадре!" << std::endl;
            cv::putText(currentFrame, "Motion Detected", cv::Point(10, 30), 
                        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
        }

        cv::imshow("Motion Detector (q to quit)", currentFrame);
        gray.copyTo(prevFrame);

        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    logFile.close();
    cap.release();
    cv::destroyAllWindows();
    
    return 0;
}
