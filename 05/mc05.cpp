//コンパイルコマンド　g++ mc05.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
// #include "../Histogram.h"  // 必要な場合はコメントアウトを外す

//関数マクロ
#define toRad(x) (x*CV_PI/180.0)
#define toDeg(x) (x*180.0/CV_PI)

// グレースケール用LUT適用
void applyLUT(cv::Mat src, cv::Mat &dst, std::vector<int> LUT)
{
    for(int j=0; j<src.rows; j++){
        for(int i=0; i<src.cols; i++){
            unsigned char x = src.at<unsigned char>(j,i);
            dst.at<unsigned char>(j,i) = LUT[x];
        }
    }
}

// カラー用LUT適用（チャンネル別）
void applyLUT(cv::Mat src, cv::Mat &dst, std::vector<int> LUT0, std::vector<int> LUT1, std::vector<int> LUT2)
{
    if(src.channels() != 3 || dst.channels() != 3){
        printf("src&dst must be a color image!\n");
        return;
    }
    for(int j=0; j<src.rows; j++){
        for(int i=0; i<src.cols; i++){
            cv::Vec3b x = src.at<cv::Vec3b>(j,i);
            x[0] = LUT0[x[0]];
            x[1] = LUT1[x[1]];
            x[2] = LUT2[x[2]];
            dst.at<cv::Vec3b>(j,i) = x;
        }
    }
}

// ========================================
// 擬似カラー変換（グレースケール → カラー）
// サーモグラフィ風：暗→青、中→緑、明→赤
// ========================================
void applyPseudoColor(cv::Mat &gray, cv::Mat &dst)
{
    dst = cv::Mat(gray.size(), CV_8UC3);
    for(int j=0; j<gray.rows; j++){
        for(int i=0; i<gray.cols; i++){
            int x = gray.at<unsigned char>(j,i);
            int r, g, b;
            // 0-127: R=0, G=0→255, B=255→0
            // 128-255: R=0→255, G=255→0, B=0
            if(x < 128){
                b = 255 - 2*x;
                g = 2*x;
                r = 0;
            } else {
                b = 0;
                g = 255 - 2*(x-128);
                r = 2*(x-128);
            }
            dst.at<cv::Vec3b>(j,i) = cv::Vec3b(
                (unsigned char)std::min(255,std::max(0,b)),
                (unsigned char)std::min(255,std::max(0,g)),
                (unsigned char)std::min(255,std::max(0,r))
            );
        }
    }
}

// ========================================
// エンボス加工
// g = negaImage（1画素ずらしたネガ）+ grayImage - 128
// ========================================
void applyEmboss(cv::Mat &gray, cv::Mat &nega, cv::Mat &dst)
{
    // negaをdxだけ平行移動
    int dx = 5, dy = 5;
    cv::Mat shifted = cv::Mat::zeros(gray.size(), CV_8UC1);
    for(int j=0; j<gray.rows; j++){
        for(int i=0; i<gray.cols; i++){
            int sj = j - dy;
            int si = i - dx;
            if(sj >= 0 && si >= 0 && sj < nega.rows && si < nega.cols){
                shifted.at<unsigned char>(j,i) = nega.at<unsigned char>(sj,si);
            } else {
                shifted.at<unsigned char>(j,i) = 128;
            }
        }
    }
    // g = f1 + f2 - 128
    cv::addWeighted(gray, 1.0, shifted, 1.0, -128, dst);
}

int main (int argc, const char* argv[])
{
    //内蔵カメラからビデオキャプチャ"capture"生成
    cv::VideoCapture capture(0);
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }

    cv::Mat frameImage;
    cv::namedWindow("Frame");

    capture >> frameImage;
    cv::resize(frameImage, frameImage, cv::Size(), 0.5, 0.5);

    // 処理結果画像の準備
    cv::Mat grayImage    = cv::Mat(frameImage.size(), CV_8UC1);
    cv::Mat negaImage    = cv::Mat(frameImage.size(), CV_8UC1);
    cv::Mat contrastImage= cv::Mat(frameImage.size(), CV_8UC3);
    cv::Mat channelImage = cv::Mat(frameImage.size(), CV_8UC3);
    cv::Mat pseudoImage  = cv::Mat(frameImage.size(), CV_8UC3);
    cv::Mat hsvImage     = cv::Mat(frameImage.size(), CV_8UC3);
    cv::Mat embossImage  = cv::Mat(frameImage.size(), CV_8UC1);
    cv::Mat trajectoryImage = cv::Mat(frameImage.size(), CV_8UC3, cv::Scalar(0,0,0));
    cv::Mat resultImage  = cv::Mat(frameImage.size(), CV_8UC3);

    // ウィンドウ配置
    cv::namedWindow("Contrast");
    cv::moveWindow("Contrast", frameImage.cols, 0);
    cv::namedWindow("PseudoColor");
    cv::moveWindow("PseudoColor", 0, frameImage.rows);
    cv::namedWindow("HSV");
    cv::moveWindow("HSV", frameImage.cols, frameImage.rows);
    cv::namedWindow("Emboss");
    cv::moveWindow("Emboss", 0, frameImage.rows*2);
    cv::namedWindow("Trajectory");
    cv::moveWindow("Trajectory", frameImage.cols, frameImage.rows*2);

    // ========================================
    // LUTテーブルの用意
    // ========================================
    std::vector<int> LUTnega(256), LUTgamma(256), LUTsigmoid(256);
    // チャンネル別LUT（コントラスト強調・夕暮れ風）
    std::vector<int> LUT_B(256), LUT_G(256), LUT_R(256);
    // 擬似カラー用 (identity)
    std::vector<int> LUTidentity(256);

    for(int x=0; x<256; x++){
        int y;

        // ネガポジ反転
        LUTnega[x] = 255 - x;

        // ガンマ補正 y=255*(x/255)^(1/2)
        y = (int)(255.0 * pow(x/255.0, 0.5));
        LUTgamma[x] = std::min(255, std::max(0, y));

        // シグモイド（コントラスト強調）
        double a = 0.05;
        int x0 = 127;
        y = (int)(1.0/(1.0+exp(-a*(x-x0)))*256);
        LUTsigmoid[x] = std::min(255, std::max(0, y));

        // identity
        LUTidentity[x] = x;

        // 夕暮れ風: Rのみガンマで明るく、G/Bはそのまま
        LUT_R[x] = LUTgamma[x];  // R を明るく
        LUT_G[x] = x;
        LUT_B[x] = x;
    }

    // ========================================
    // 軌跡表示用（過去nフレームの累積）
    // ========================================
    const int trajectoryAlpha_num = 10; // フレーム数
    double trajectoryDecay = 0.85;       // 残像の減衰率

    while(1){
        capture >> frameImage;
        if(frameImage.empty()) break;
        cv::resize(frameImage, frameImage, cv::Size(), 0.5, 0.5);

        // グレースケール変換
        cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);

        // ----------------------------------------
        // 1. コントラスト強調（シグモイドLUT、全チャンネル共通）
        // ----------------------------------------
        applyLUT(frameImage, contrastImage, LUTsigmoid, LUTsigmoid, LUTsigmoid);

        // ----------------------------------------
        // 2. 特定チャンネル強調（夕暮れ風：Rのみガンマ補正）
        // ----------------------------------------
        applyLUT(frameImage, channelImage, LUT_B, LUT_G, LUT_R);

        // ----------------------------------------
        // 3. 擬似カラー（サーモグラフィ風）
        // ----------------------------------------
        applyPseudoColor(grayImage, pseudoImage);

        // ----------------------------------------
        // 4. HSV加工（色相シフト＋彩度アップ）
        // ----------------------------------------
        {
            cv::Mat hsv;
            cv::cvtColor(frameImage, hsv, cv::COLOR_BGR2HSV);
            for(int j=0; j<hsv.rows; j++){
                for(int i=0; i<hsv.cols; i++){
                    cv::Vec3b &p = hsv.at<cv::Vec3b>(j,i);
                    // H: 色相を90°シフト（0-179の範囲）
                    p[0] = (p[0] + 45) % 180;
                    // S: 彩度を上げる（ポスタリゼーション風）
                    p[1] = std::min(255, (int)(p[1] * 1.5));
                }
            }
            cv::cvtColor(hsv, hsvImage, cv::COLOR_HSV2BGR);
        }

        // ----------------------------------------
        // 5. エンボス加工
        // ----------------------------------------
        applyLUT(grayImage, negaImage, LUTnega);
        applyEmboss(grayImage, negaImage, embossImage);

        // ----------------------------------------
        // 6. 軌跡（アルファブレンディング）
        // 現フレームをカラーのまま軌跡画像にブレンド
        // ----------------------------------------
        cv::addWeighted(trajectoryImage, trajectoryDecay, frameImage, 1.0 - trajectoryDecay, 0, trajectoryImage);

        // ----------------------------------------
        // Result: コントラスト強調 + 擬似カラー を並べる代わりに
        //         ここでは emboss をカラー化して表示
        // ----------------------------------------
        cv::cvtColor(embossImage, resultImage, cv::COLOR_GRAY2BGR);

        // 画像表示
        cv::imshow("Frame",       grayImage);
        cv::imshow("Contrast",    contrastImage);   // コントラスト強調（夕暮れ風）
        cv::imshow("PseudoColor", pseudoImage);     // 擬似カラー
        cv::imshow("HSV",         hsvImage);        // HSV加工
        cv::imshow("Emboss",      embossImage);     // エンボス
        cv::imshow("Trajectory",  trajectoryImage); // 軌跡

        char key = cv::waitKey(10);
        if(key == 'q' || key == 'Q') break;

        // キーによるモード切り替え（おまけ）
        // 's': コントラスト強調をResultに
        // 'c': 擬似カラーをResultに
        // 'h': HSV加工をResultに
        if(key == 's') cv::imshow("Frame", contrastImage);
        if(key == 'c') cv::imshow("Frame", pseudoImage);
        if(key == 'h') cv::imshow("Frame", hsvImage);
    }

    return 0;
}
