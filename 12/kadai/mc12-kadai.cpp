// g++ -std=c++11 mc12-kadai.cpp `pkg-config --cflags --libs opencv4`
// 第12回課題: 人の顔以外のカスケード分類器を探して使う
// → OpenCV公式配布の「猫の正面顔」検出器 haarcascade_frontalcatface_extended.xml を使用
//   出典: https://github.com/opencv/opencv/tree/master/data/haarcascades
//   (Joseph Howse 作, LBP/Haar ベースの猫顔検出器)
//
// 使い方:
//   ./a.out 画像ファイル   → 静止画から猫顔検出、結果を images/catResult.jpg に保存
//   ./a.out               → 内蔵カメラ映像からリアルタイム検出 (qで終了)
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>

// 検出して矩形を描画する共通処理
static int detectAndDraw(cv::CascadeClassifier &cascade, const cv::Mat &src, cv::Mat &dst) {
    dst = src.clone();
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);  // 明るさを正規化して検出精度を上げる

    std::vector<cv::Rect> cats;
    // 猫顔検出器は人の顔用より検出が渋いため、拡大率を1.02と細かくして取り逃しを減らす
    // (1.1 では images/cat2.jpg の猫顔を取り逃した)
    // 一方で誤検出も出やすいので近傍数を8まで上げて抑制 (3ではしっぽを誤検出)
    cascade.detectMultiScale(gray, cats, 1.02, 8, 0, cv::Size(100, 100));

    for (size_t i = 0; i < cats.size(); i++) {
        cv::rectangle(dst, cats[i], cv::Scalar(0, 0, 255), 3);
        cv::putText(dst, "cat", cv::Point(cats[i].x, cats[i].y - 8),
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }
    return (int)cats.size();
}

int main(int argc, char *argv[]) {
    cv::CascadeClassifier cascade;
    cascade.load("cascade/haarcascade_frontalcatface_extended.xml");
    if (cascade.empty()) {
        printf("Cascade file not found\n");
        return -1;
    }

    if (argc >= 2) {
        // 静止画モード
        cv::Mat srcImg = cv::imread(argv[1]);
        if (srcImg.empty()) {
            printf("Image file not found\n");
            return -1;
        }
        cv::Mat resultImg;
        int n = detectAndDraw(cascade, srcImg, resultImg);
        printf("detected: %d cat face(s)\n", n);
        cv::imwrite("images/catResult.jpg", resultImg);
        if (getenv("HEADLESS") == NULL) {
            cv::imshow("result", resultImg);
            cv::waitKey(0);
        }
        return 0;
    }

    // カメラモード (mc12-2.cpp と同様)
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        printf("Camera not found\n");
        return -1;
    }
    int width = 720, height = 400;
    cv::Mat captureImage, resultImg;
    cv::Mat frameImage(cv::Size(width, height), CV_8UC3);
    while (1) {
        capture >> captureImage;
        cv::resize(captureImage, frameImage, frameImage.size());
        detectAndDraw(cascade, frameImage, resultImg);
        cv::imshow("result", resultImg);
        char key = cv::waitKey(30);
        if (key == 'q' || key == 'Q') break;
    }
    return 0;
}
