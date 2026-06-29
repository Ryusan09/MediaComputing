// g++ -std=c++11 mc07.cpp `pkg-config --cflags --libs opencv4`
//https://qiita.com/hmichu/items/f5f1c778a155c7c414fd
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
#include <opencv2/features2d.hpp>//特徴点マッチング用ヘッダ

int main (int argc, char *argv[])
{
    // 画像の読み込み
    cv::Mat Source1 = cv::imread("images/panorama1.jpg", cv::IMREAD_COLOR);
    if (Source1.empty()) {
        std::cout << "Failed to open Source image1" << std::endl;
        return -1;
    }

    cv::Mat Source2 = cv::imread("images/panorama2.jpg", cv::IMREAD_COLOR);
    if (Source2.empty()) {
        std::cout << "Failed to open Source image2" << std::endl;
        return -1;
    }

    // 画像を配列に格納
    std::vector<cv::Mat> Sources;
    Sources.push_back(Source1);
    Sources.push_back(Source2);

    //入力画像をグレースケール化
    cv::Mat gray0, gray1;
    cv::cvtColor(Sources[0], gray0,cv::COLOR_BGR2GRAY);
    cv::cvtColor(Sources[1], gray1,cv::COLOR_BGR2GRAY);
    std::vector<cv::Mat> gray;
    gray.push_back(gray0);
    gray.push_back(gray1);
    
    //結果画像
    cv::Mat resultImage;

    //特徴点検出
    //検出方法の指定とモジュール生成
    cv::Ptr<cv::Feature2D> features = cv::SIFT::create();

    //画像毎の特徴点を格納するための配列
    std::vector<cv::KeyPoint> keyPoints[2];
    //画像毎の特徴量を格納するための配列
    cv::Mat descriptors[2];

    // それぞれの入力画像に対してdetectAndCompute関数を用いて特徴点と特徴量を算出
    features->detectAndCompute(gray[0], cv::noArray(), keyPoints[0], descriptors[0]);
    features->detectAndCompute(gray[1], cv::noArray(), keyPoints[1], descriptors[1]);

    //特徴量マッチング
    cv::BFMatcher matcher;

    //最も良い点を探す
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors[0], descriptors[1], matches);
    
    //見つかった対応点の表示
    cv::drawMatches(Sources[0], keyPoints[0], Sources[1], keyPoints[1], matches, resultImage);
    
    // 対応点の選別
    
    //結果表示
    cv::imshow("source0", Sources[0]);
    cv::imshow("source1", Sources[1]);
    cv::imshow("result", resultImage);

    //キー入力待ち
    cv::waitKey(0);

    return 0;
}
