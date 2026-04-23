// g++ dip02-2.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

using namespace std;
using namespace cv;

int main(int argc, const char *argv[])
{
    // ①カメラオープン
    VideoCapture cap(0); // ポート0から映像取り込み
    // ①’ 動画ファイルの場合
    //    VideoCapture cap("******.mov");  // 動画ファイル名を記載．movやmp4など大抵の形式に対応
    if (cap.isOpened() == 0)
    { // カメラやムービーをセットできなかった場合
        printf("Camera not found\n");
        exit(0);
    }
    // ②画像格納用インスタンスの生成
    Mat frameImage;
    // １フレーム取り出し（サイズを調べたりするため）
    cap >> frameImage;

    printf("Width=%d, Height=%d\n", frameImage.cols, frameImage.rows);
    printf("Channel=%d, Depth=%d\n", frameImage.channels(), frameImage.depth());
    // 画像が大きすぎるので半分にリサイズ resize(入力画像，出力画像，Size(), X拡大率，Y拡大率)
    resize(frameImage, frameImage, Size(), 0.5, 0.5);
    printf("Width=%d, Height=%d\n", frameImage.cols, frameImage.rows);

    // ②’ 結果格納用インスタンスの生成（frameImageのサイズが分かったあとで確保．while文の中では確保しないこと．）
    Mat resultImage = Mat(frameImage.size(), CV_8UC3); // frameImageと同じサイズのカラー画像
    Mat hsvImage;

    // ③ウィンドウの生成と移動
    namedWindow("Frame");

    // ⑤ビデオライタ生成(ファイル名，コーデック，フレームレート，フレームサイズ)
    cv::VideoWriter rec("rec.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, resultImage.size());

    while (1)
    { // 無限ループ
        // １フレーム取り出し
        cap >> frameImage;
        // 画像が大きすぎるので半分にリサイズ resize(入力画像，出力画像，Size(), X拡大率，Y拡大率)
        resize(frameImage, frameImage, Size(), 0.5, 0.5);

        // ④画像処理
        resultImage = frameImage.clone();

        // BGR->HSV
        cvtColor(frameImage, hsvImage, COLOR_BGR2HSV);
        // H, S, Vをいじってみる
        for (int j = 0; j < hsvImage.rows; j++)
        {
            for (int i = 0; i < hsvImage.cols; i++)
            {
                Vec3b s;                      // sはバイト型（8ビット）3次元ベクトル＝3チャンネル画素値
                s = hsvImage.at<Vec3b>(j, i); // hsvImageの画素(i, j)の画素値の読み取り
                // H（色相）: 約 19
                // S（彩度）: 約 64 
                // V（明度）: 約 142
                if (s[0]>0 && s[0]<25 && s[1]>50 && s[1]<255 && s[2]>40 && s[2]<255)
                {
                    s[0] = s[0];     // Hue
                    s[1] = s[1];     // Saturation
                    s[2] = s[2];  // Value
                }
                else
                {
                    s[0] = s[0];                        // Hue
                    s[1] = s[1]/100;                        // Saturation
                    s[2] = s[2]/3;                     // Value
                }
                resultImage.at<Vec3b>(j, i) = s; // resultImageの画素(i, j)に画素値sを代入
            }
        }
        // HSV->BGR RGB色空間に戻す
        cvtColor(resultImage, resultImage, COLOR_HSV2BGR);

        // ⑤ウィンドウへの画像の表示
        cv::imshow("Frame", frameImage);
        cv::imshow("Result", resultImage);

        // フレームの書き出し
        rec << resultImage;

        // ⑥キー入力待ち
        char key = waitKey(1); // 33msec待機 → 30fpsに相当
        if (key == 'q')
            break; // qキーが入力されたらループ脱出
    }

    // ⑦ キャプチャーとライターの解放
    cap.release();
    rec.release();

    return 0;
}
