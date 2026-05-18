//コンパイルコマンド　g++ mc04.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

int main (int argc, const char* argv[])
{
    //内蔵カメラからビデオキャプチャ"capture"生成
    cv::VideoCapture capture(0);  //"water.mov"にすればムービーファイルから映像を取り込む
    //キャプチャできたかチェック
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }

    //画像サイズの指定
    int width = 720, height = 405;
    
    //画像格納用インスタンス準備
    cv::Mat captureImage;  //オリジナルの入力画像格納用（大抵大きすぎる）
    cv::Mat frameImage(cv::Size(width, height), CV_8UC3);  //コンストラクタによる宣言時のインスタンス確保
    //cv::Mat frameImage = cv::Mat(cv::Size(width, height), CV_8UC3);  //こう書いても良い．宣言した後にインスタンス設定

    //ウィンドウの生成
    cv::namedWindow("Frame");

    while(1){
        //カメラから1フレーム取り出し
        capture >> captureImage;

        //画像の大きさ変更
        cv::resize(captureImage, frameImage, frameImage.size());

        //画像表示
        cv::imshow("Frame", frameImage);
        
        char key = cv::waitKey(20);
        if(key == 'q' || key == 'Q') break;
    }

    return 0;
}
