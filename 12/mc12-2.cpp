// g++ -std=c++11 mc12-2.cpp `pkg-config --cflags --libs opencv4`
//https://algorithm.joho.info/programming/python/opencv-haar-cascade-face-detection-py/
//https://github.com/opencv/opencv/tree/master/data/haarcascades
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

int main (int argc, char *argv[])
{
    //内蔵カメラからビデオキャプチャ"capture"生成
    cv::VideoCapture capture(0);
    //カメラがあるかチェック
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }

    //画像サイズの指定
	int width = 720, height = 400;
    cv::Mat captureImage;
    cv::Mat frameImage(cv::Size(width, height), CV_8UC3);
    cv::Mat resultImg;

    //カスケード型識別器
    cv::CascadeClassifier cascade;
    cascade.load("cascade/haarcascade_frontalface_default.xml");
    if(cascade.empty()) {
        printf("Cascade file not found\n");
        exit(0);
    }
    
    while(1){
		//カメラから1フレーム取り出し
		capture >> captureImage;

		//画像の大きさ変更
		cv::resize(captureImage, frameImage, frameImage.size());
        resultImg = frameImage.clone();

        //検索結果の格納用
        std::vector<cv::Rect> objects;
        //カスケード型識別器の利用
        //cascade.detectMultiScale(入力画像, 結果格納先, 検索窓の拡大率, 近傍の最小サイズ);
        cascade.detectMultiScale(frameImage, objects, 1.1, 3);
        //対象領域を表示する
        std::vector<cv::Rect>::const_iterator iter = objects.begin();
        while(iter!=objects.end()) {
            cv::rectangle(resultImg,
                cv::Rect(iter->x, iter->y, iter->width, iter->height),
                cv::Scalar(0, 0, 255), 5);
            ++iter;
        }

		//画像表示
        cv::imshow("result", resultImg);
		
		char key = cv::waitKey(30);
		if(key == 'q' || key == 'Q') break;
	}
    return 0;
}   
