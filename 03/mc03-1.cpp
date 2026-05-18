//コンパイルコマンド　g++ mc03-1.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
#include <string>
#include "Stats.h"

using namespace std;
using namespace cv;

using namespace gray;
using namespace coler;

int main (int argc, const char* argv[]) {
    //画像ファイルの読み込み
    Mat sImage = imread("./red.png", IMREAD_COLOR); // 適当な画像に変える
    if (sImage.data==0) {  //画像ファイルが読み込めなかった場合
        printf("File not found\n");
        exit(0);
    }
    printf("W=%d, H=%d\n", sImage.cols, sImage.rows);
    printf("C=%d, D=%d\n", sImage.channels(), sImage.depth());

	Mat grayImage;
	//BGR->Gray
	cvtColor(sImage, grayImage, COLOR_BGR2GRAY);

    //ウィンドウの生成と表示
    namedWindow("Source");
    namedWindow("Gray");
    imshow("Source", sImage);
    imshow("Gray", grayImage);

	//ヒストグラム画像の生成（グレースケール）
    vector<int> histogram;
    vector<vector<int>> color_histogram;
    gray::make_histogram(grayImage, histogram);
    coler::make_histgram(sImage, color_histogram);
    gray::show_histogram("Histogram", histogram, Size(256, 256));
    coler::show_histogram("Histogram", color_histogram, Size(256, 256));
    // カラーヒストグラムの生成（できる人はやってみよう）

    // キー入力待ち
    waitKey(0);

    return 0;
}