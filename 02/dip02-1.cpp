//g++ dip02-1.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

using namespace std;
using namespace cv;

int main (int argc, const char* argv[])
{
    //①画像ファイルの読み込み
	Mat sourceImage = imread("image1.jpg", IMREAD_UNCHANGED);  //アルファチャンネルがあってもそのまま読み込んでくれる
    if (sourceImage.data==0) {  //画像ファイルが読み込めなかった場合
        printf("File not found\n");
        exit(0);
    }
    printf("Width=%d, Height=%d\n", sourceImage.cols, sourceImage.rows);
    printf("Channel=%d, Depth=%d\n", sourceImage.channels(), sourceImage.depth());
    // 画像が大きすぎるので半分にリサイズ resize(入力画像，出力画像，Size(), X拡大率，Y拡大率)
    resize(sourceImage, sourceImage, Size(), 0.25, 0.25);
    printf("Width=%d, Height=%d\n", sourceImage.cols, sourceImage.rows);

    //②画像格納用インスタンスの生成
    Mat resultImage = Mat(sourceImage.size(), CV_8UC3);  // sourceImageと同じサイズのカラー画像
	
    //③ウィンドウの生成と移動
    namedWindow("Source");

    //④画像処理
    // resultImage = sourceImage.clone();  // とりあえず入力画像をそのままコピー（実際は何か処理した結果を格納する）
    // 画像の色変換
    Mat grayImage, hsvImage; // 画像格納用
    // BGR->GRAY (cvtColorは出力画像の配列を自動で確保してくれる）
    cvtColor(sourceImage, grayImage, COLOR_BGR2GRAY);
    // BGR->HSV
    cvtColor(sourceImage, hsvImage, COLOR_BGR2HSV);
    // H, S, Vをいじってみる
    for (int j = 0; j < hsvImage.rows; j++)
    {
        for (int i = 0; i < hsvImage.cols; i++)
        {
            Vec3b s;                         // sはバイト型（8ビット）3次元ベクトル＝3チャンネル画素値
            s = hsvImage.at<Vec3b>(j, i);    // hsvImageの画素(i, j)の画素値の読み取り
            s[0] = s[0];                        // Hue
            s[1] = s[1]/5;                     // Saturation
            s[2] = s[2];                     // Value
            resultImage.at<Vec3b>(j, i) = s; // resultImageの画素(i, j)に画素値sを代入
        }
    }
    // HSV->BGR RGB色空間に戻す
    cvtColor(resultImage, resultImage, COLOR_HSV2BGR);
    // ⑤ウィンドウへの画像の表示
    cv::imshow("Source", sourceImage);
    // cv::imshow("Gray", grayImage);
    // cv::imshow("HSV", hsvImage);

    //⑤ウィンドウへの画像の表示
    cv::imshow("Source", sourceImage);
    cv::imshow("Result", resultImage);

    //⑥キー入力待ち
    cv::waitKey(0);
    
    //⑦画像の保存
    imwrite("result.jp", resultImage);
    
    return 0;
}
