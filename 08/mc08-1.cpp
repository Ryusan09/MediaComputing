// g++ mc08-1.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>     //入出力関連ヘッダ
#include <opencv2/opencv.hpp>   //OpenCV関連ヘッダ

// ミーンシフト法のパラメータ
#define SP (20.0)          //2〜40
#define SR (20.0)        //2〜40
#define MAX_LEVEL (2)   //1〜3

int main (int argc, const char* argv[])
{
    //画像ファイルの読み込み
    cv::Mat srcImage = cv::imread("images/ait.png", cv::IMREAD_COLOR);
    if (srcImage.data == 0) {  //画像ファイルが読み込めなかった場合
        printf("File not found\n");
        exit(0);
    }
    
    //ミーンシフト法の実行
    cv::Mat dstImage = srcImage.clone();
    cv::pyrMeanShiftFiltering(srcImage, dstImage, SP, SR, MAX_LEVEL,
    cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 5, 1));
    
    //画像の表示
    cv::imshow("Source", srcImage);
    cv::imshow("Destination", dstImage);
    
    cv::waitKey (0);

    //画像の保存
    // cv::imwrite("images/resultAIT.jpg", dstImage);

    return 0;
}
