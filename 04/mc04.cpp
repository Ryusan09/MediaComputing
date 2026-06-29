//コンパイルコマンド　g++ mc04.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

// トラックバー用グローバル変数
int g_gamma100  = 200;  // γ × 100 (初期値 γ=2.0)
int g_sigmoid_a = 5;    // sigmoid a × 100 (初期値 a=0.05)
int g_poster_p  = 4;    // ポスタリゼーション階調数

// ── 演習04: LUT作成関数群 ──────────────────────────

// 折れ線: y = 2x (上限255)
void makeLUT_piecewise(cv::Mat& lut) {
    lut = cv::Mat(1, 256, CV_8UC1);
    for (int x = 0; x < 256; x++)
        lut.at<uchar>(0, x) = (uchar)std::min(2 * x, 255);
}

// ガンマ変換: y = 255 × (x/255)^(1/γ)
void makeLUT_gamma(cv::Mat& lut, double gamma) {
    lut = cv::Mat(1, 256, CV_8UC1);
    for (int x = 0; x < 256; x++) {
        double y = (x == 0) ? 0.0 : 255.0 * std::pow(x / 255.0, 1.0 / gamma);
        lut.at<uchar>(0, x) = cv::saturate_cast<uchar>((int)std::round(y));
    }
}

// S字型 (シグモイド): y = 255 / (1 + exp(-a*(x-128)))
void makeLUT_sigmoid(cv::Mat& lut, double a) {
    lut = cv::Mat(1, 256, CV_8UC1);
    for (int x = 0; x < 256; x++) {
        double y = 255.0 / (1.0 + std::exp(-a * (x - 128.0)));
        lut.at<uchar>(0, x) = cv::saturate_cast<uchar>((int)std::round(y));
    }
}

// ネガポジ反転: y = 255 - x
void makeLUT_negative(cv::Mat& lut) {
    lut = cv::Mat(1, 256, CV_8UC1);
    for (int x = 0; x < 256; x++)
        lut.at<uchar>(0, x) = (uchar)(255 - x);
}

// ポスタリゼーション: 階調数 p で制御
void makeLUT_posterize(cv::Mat& lut, int p) {
    lut = cv::Mat(1, 256, CV_8UC1);
    int step = 256 / p;
    for (int x = 0; x < 256; x++) {
        int y = (x / step) * step;
        lut.at<uchar>(0, x) = (uchar)std::min(y, 255);
    }
}

// ── 課題04: HSVポスタリゼーション (アニメ調) ──────────

cv::Mat applyHSVPosterize(const cv::Mat& bgr, int p) {
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    cv::Mat lutP;
    makeLUT_posterize(lutP, p);

    std::vector<cv::Mat> ch;
    cv::split(hsv, ch);
    cv::LUT(ch[1], lutP, ch[1]);  // Saturation
    cv::LUT(ch[2], lutP, ch[2]);  // Value
    cv::merge(ch, hsv);

    cv::Mat result;
    cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
    return result;
}

// ラベル描画 (縁取り付き)
void putLabel(cv::Mat& img, const std::string& text) {
    cv::putText(img, text, cv::Point(5, 22),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 3);
    cv::putText(img, text, cv::Point(5, 22),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);
}

int main (int argc, const char* argv[])
{
    cv::VideoCapture capture(0);
    if (capture.isOpened() == 0) {
        printf("Camera not found\n");
        return -1;
    }

    // セルサイズ (2行×3列グリッド用)
    int cellW = 360, cellH = 203;

    cv::Mat captureImage;
    cv::Mat frameImage(cv::Size(cellW, cellH), CV_8UC3);

    cv::namedWindow("ToneCurves");

    // トラックバー
    cv::createTrackbar("Gamma x100",     "ToneCurves", &g_gamma100,  500);
    cv::createTrackbar("Sigmoid a x100", "ToneCurves", &g_sigmoid_a, 100);
    cv::createTrackbar("Poster Levels",  "ToneCurves", &g_poster_p,   16);

    while(1) {
        capture >> captureImage;
        cv::resize(captureImage, frameImage, frameImage.size());

        // トラックバーから値を取得
        double gamma   = std::max(0.1,   g_gamma100  / 100.0);
        double sigA    = std::max(0.001, g_sigmoid_a / 100.0);
        int    posterP = std::max(2,     g_poster_p);

        // LUT作成
        cv::Mat lutPW, lutG, lutSig, lutNeg, lutPoster;
        makeLUT_piecewise(lutPW);
        makeLUT_gamma(lutG, gamma);
        makeLUT_sigmoid(lutSig, sigA);
        makeLUT_negative(lutNeg);
        makeLUT_posterize(lutPoster, posterP);

        // 各変換を適用
        cv::Mat pwImg, gammaImg, sigImg, negImg, posterImg, animeImg;
        cv::LUT(frameImage, lutPW,     pwImg);
        cv::LUT(frameImage, lutG,      gammaImg);
        cv::LUT(frameImage, lutSig,    sigImg);
        cv::LUT(frameImage, lutNeg,    negImg);
        cv::LUT(frameImage, lutPoster, posterImg);
        animeImg = applyHSVPosterize(frameImage, posterP);

        // ラベル付与
        std::string gStr = std::to_string(gamma).substr(0, 4);
        cv::Mat origImg = frameImage.clone();
        putLabel(origImg,   "Original");
        putLabel(pwImg,     "Piecewise (y=2x)");
        putLabel(gammaImg,  "Gamma (g=" + gStr + ")");
        putLabel(sigImg,    "Sigmoid");
        putLabel(negImg,    "Negative");
        putLabel(animeImg,  "Anime (HSV Poster)");

        // 2行×3列グリッドで結合して表示
        cv::Mat row1, row2, grid;
        cv::hconcat(std::vector<cv::Mat>{origImg, pwImg,  gammaImg}, row1);
        cv::hconcat(std::vector<cv::Mat>{sigImg,  negImg, animeImg}, row2);
        cv::vconcat(row1, row2, grid);

        cv::imshow("ToneCurves", grid);

        char key = cv::waitKey(20);
        if(key == 'q' || key == 'Q') break;
    }

    return 0;
}
