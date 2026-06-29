//g++ mc06_kadai.cpp -std=c++11 `pkg-config --cflags --libs opencv4` -o mc06_kadai

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, const char* argv[])
{
    //①カメラ（または動画ファイル）のオープン
    // カメラ入力の場合: cv::VideoCapture cap(0);
    // 動画ファイルの場合: cv::VideoCapture cap("input.mp4");
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        printf("Camera not found\n");
        return -1;
    }

    int width  = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    printf("Width=%d, Height=%d\n", width, height);

    int dispWidth  = width  / 2;
    int dispHeight = height / 2;

    //②画像格納用インスタンスの生成
    cv::Mat frame, grayImage;
    cv::Mat edgeImage;      // エッジ抽出（Laplacian）結果
    cv::Mat medianImage;    // メディアンフィルタ結果
    cv::Mat dispGray, dispEdge, dispMedian;

    //③ウィンドウの生成と配置
    cv::namedWindow("Original");
    cv::moveWindow("Original", 0, 0);

    cv::namedWindow("Laplacian");           // エッジ抽出
    cv::moveWindow("Laplacian", dispWidth, 0);

    cv::namedWindow("Median");              // メディアンフィルタ
    cv::moveWindow("Median", dispWidth * 2, 0);

    //④ラプラシアンフィルタ配列の作成
    //   縦方向2次微分＋横方向2次微分を合算したもの
    //   [ 0,  1,  0]
    //   [ 1, -4,  1]
    //   [ 0,  1,  0]
    float Flap[] = {
         0.0f,  1.0f,  0.0f,
         1.0f, -4.0f,  1.0f,
         0.0f,  1.0f,  0.0f,
    };
    cv::Mat lapKernel(cv::Size(3, 3), CV_32F, Flap);

    //⑤メインループ（フレームごとに処理）
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        //グレースケール変換
        cv::cvtColor(frame, grayImage, cv::COLOR_BGR2GRAY);

        //--- エッジ抽出: ラプラシアンフィルタ ---
        // filter2D は負値をクリップするため convertScaleAbs で絶対値変換
        cv::Mat lapRaw;
        cv::filter2D(grayImage, lapRaw, CV_16S, lapKernel);
        cv::convertScaleAbs(lapRaw, edgeImage);

        //--- メディアンフィルタ（カーネルサイズ15） ---
        cv::medianBlur(grayImage, medianImage, 15);

        //⑥ウィンドウへの表示（1/2サイズにリサイズ）
        cv::resize(grayImage,   dispGray,   cv::Size(dispWidth, dispHeight));
        cv::resize(edgeImage,   dispEdge,   cv::Size(dispWidth, dispHeight));
        cv::resize(medianImage, dispMedian, cv::Size(dispWidth, dispHeight));
        cv::imshow("Original",  dispGray);
        cv::imshow("Laplacian", dispEdge);
        cv::imshow("Median",    dispMedian);

        //⑦キー入力（q または ESC で終了）
        int key = cv::waitKey(1);
        if (key == 'q' || key == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
