// g++ mc08-2.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>     //入出力関連ヘッダ
#include <opencv2/opencv.hpp>   //OpenCV関連ヘッダ

//グラフカットの回数
#define ITER_COUNT (7) //7回がいい感じ

int main (int argc, const char* argv[])
{
    //画像ファイルの読み込み
	cv::Mat srcImage = cv::imread("images/messi.png", cv::IMREAD_COLOR);
    if (srcImage.data==0) {  //画像ファイルが読み込めなかった場合
        printf("File not found\n");
        exit(0);
    }

    //矩形の生成
    cv::Rect rectangle(100, 5, 130, 190);

    //グラフカットの実行
    cv::Mat result, bgModel, fgModel;
    cv::grabCut(srcImage, result, rectangle, bgModel, fgModel, ITER_COUNT, cv::GC_INIT_WITH_RECT);
    cv::compare(result, cv::GC_PR_FGD, result, cv::CMP_EQ);

    // マスク処理の実行
    cv::Mat fgImage(srcImage.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    srcImage.copyTo(fgImage, result);

    //画像の表示
    cv::rectangle(srcImage, rectangle, cv::Scalar(0, 0, 255), 1);
    cv::imshow("Source", srcImage);
    cv::imshow("Result", result);
    cv::imshow("Foreground", fgImage);

    cv::waitKey(0);

    return 0;
}
