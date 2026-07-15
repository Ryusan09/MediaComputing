// g++ -std=c++11 mc12-kadai2.cpp `pkg-config --cflags --libs opencv4`
// 第12回課題(その2): 人の顔以外のカスケード分類器 — 車両検出
// 分類器: cars.xml (Andrews Sobral, vehicle_detection_haarcascades)
//   出典: https://github.com/andrewssobral/vehicle_detection_haarcascades
//   旧形式(haartraining)のHaarカスケードだが OpenCV 4.13 でも読み込み可能だった
//
// 使い方: ./a.out 画像ファイル → 結果を images/carResult.jpg に保存
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>

int main(int argc, char *argv[]) {
    cv::CascadeClassifier cascade;
    if (!cascade.load("cars.xml")) {
        printf("Cascade file not found\n");
        return -1;
    }
    cv::Mat srcImg = cv::imread(argc >= 2 ? argv[1] : "images/traffic.jpg");
    if (srcImg.empty()) {
        printf("Image file not found\n");
        return -1;
    }

    cv::Mat gray;
    cv::cvtColor(srcImg, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);

    std::vector<cv::Rect> cars;
    // 猫顔と同様 scaleFactor=1.1 では全滅。1.05 に下げると検出できる
    // 近傍数3。2でも結果はほぼ同じ (highway.jpg で 20件 vs 18件)
    cascade.detectMultiScale(gray, cars, 1.05, 3, 0, cv::Size(40, 40));

    cv::Mat resultImg = srcImg.clone();
    for (size_t i = 0; i < cars.size(); i++) {
        cv::rectangle(resultImg, cars[i], cv::Scalar(0, 255, 0), 3);
        cv::putText(resultImg, "car", cv::Point(cars[i].x, cars[i].y - 8),
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
    }
    printf("detected: %zu car(s)\n", cars.size());
    cv::imwrite("images/carResult.jpg", resultImg);
    if (getenv("HEADLESS") == NULL) {
        cv::imshow("result", resultImg);
        cv::waitKey(0);
    }
    return 0;
}
