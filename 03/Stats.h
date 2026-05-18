#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ
#include <string>
#include <cmath>

using namespace std;
using namespace cv;

namespace gray
{
    void make_histogram(const cv::Mat src, std::vector<int> &hArray)
    {
        // ヒストグラムの初期化
        hArray.assign(256, 0);

        if (src.channels() != 1)
        {
            cerr << "Input image must be a grayscale image." << std::endl;
            return;
        }

        // 画像の全ピクセルを走査してヒストグラムを作成
        for (int y = 0; y < src.rows; y++)
        {
            for (int x = 0; x < src.cols; x++)
            {
                int pixelValue = src.at<uchar>(y, x); // グレースケール画像の場合
                hArray[pixelValue]++;
            }
        }
    }

    void show_histogram(const std::string winname, const std::vector<int> hArray, cv::Size hSize)
    {
        // ヒストグラム画像の生成
        int maxVal = *std::max_element(hArray.begin(), hArray.end());
        cv::Mat histImage(hSize, CV_8UC3, cv::Scalar(0, 0, 0)); // 黒背景のヒストグラム画像

        for (int i = 0; i < (int)hArray.size() - 1; i++)
        {
            int height = static_cast<int>((hArray[i] / static_cast<double>(maxVal)) * hSize.height);
            int height2 = static_cast<int>((hArray[i + 1] / static_cast<double>(maxVal)) * hSize.height);
            cv::line(histImage, cv::Point(i, hSize.height - height), cv::Point(i + 1, hSize.height - height2), cv::Scalar(255, 255, 255), 1);
        }

        // ヒストグラム画像の表示
        cv::imshow(winname, histImage);
    }

    // --- 全画像統計量 ---

    int min(cv::Mat src)
    {
        int minVal = 255;
        for (int y = 0; y < src.rows; y++)
            for (int x = 0; x < src.cols; x++)
                if (src.at<uchar>(y, x) < minVal)
                    minVal = src.at<uchar>(y, x);
        return minVal;
    }

    int max(cv::Mat src)
    {
        int maxVal = 0;
        for (int y = 0; y < src.rows; y++)
            for (int x = 0; x < src.cols; x++)
                if (src.at<uchar>(y, x) > maxVal)
                    maxVal = src.at<uchar>(y, x);
        return maxVal;
    }

    double mean(cv::Mat src)
    {
        double sum = 0.0;
        int total = src.rows * src.cols;
        for (int y = 0; y < src.rows; y++)
            for (int x = 0; x < src.cols; x++)
                sum += src.at<uchar>(y, x);
        return sum / total;
    }

    int median(cv::Mat src)
    {
        std::vector<int> hist;
        make_histogram(src, hist);
        int total = src.rows * src.cols;
        int cumulative = 0;
        for (int i = 0; i < 256; i++)
        {
            cumulative += hist[i];
            if (cumulative >= total / 2)
                return i;
        }
        return 255;
    }

    int mode(cv::Mat src)
    {
        std::vector<int> hist;
        make_histogram(src, hist);
        return (int)(std::max_element(hist.begin(), hist.end()) - hist.begin());
    }

    double variance(cv::Mat src)
    {
        double m = mean(src);
        double sum = 0.0;
        int total = src.rows * src.cols;
        for (int y = 0; y < src.rows; y++)
            for (int x = 0; x < src.cols; x++)
            {
                double diff = src.at<uchar>(y, x) - m;
                sum += diff * diff;
            }
        return sum / total;
    }

    double stddev(cv::Mat src)
    {
        return std::sqrt(variance(src));
    }

}


namespace coler
{
    void make_histgram(const cv::Mat src, std::vector<std::vector<int>> &hArray)
    {
        // カラーヒストグラムの初期化
        hArray.assign(3, std::vector<int>(256, 0));

        // 画像の全ピクセルを走査してヒストグラムを作成
        for (int y = 0; y < src.rows; y++)
        {
            for (int x = 0; x < src.cols; x++)
            {
                cv::Vec3b pixelValue = src.at<cv::Vec3b>(y, x); // カラー画像の場合
                hArray[0][pixelValue[0]]++;                     // Blueチャンネル
                hArray[1][pixelValue[1]]++;                     // Greenチャンネル
                hArray[2][pixelValue[2]]++;                     // Redチャンネル
            }
        }
    }

    void show_histogram(const std::string winname, const std::vector<std::vector<int>> hArray, cv::Size hSize)
    {
        // ヒストグラム画像の生成
        int B_maxVal = *std::max_element(hArray[0].begin(), hArray[0].end());
        int G_maxVal = *std::max_element(hArray[1].begin(), hArray[1].end());
        int R_maxVal = *std::max_element(hArray[2].begin(), hArray[2].end());
        cv::Mat histImage(hSize, CV_8UC3, cv::Scalar(0, 0, 0)); // 黒背景のヒストグラム画像

        for (int i = 0; i < (int)hArray[0].size() - 1; i++)
        {
            // B
            int B_height = static_cast<int>((hArray[0][i] / static_cast<double>(B_maxVal)) * hSize.height);
            int B_height2 = static_cast<int>((hArray[0][i + 1] / static_cast<double>(B_maxVal)) * hSize.height);
            cv::line(histImage, cv::Point(i, hSize.height - B_height), cv::Point(i + 1, hSize.height - B_height2), cv::Scalar(255, 0, 0), 1);

            // G
            int G_height = static_cast<int>((hArray[1][i] / static_cast<double>(G_maxVal)) * hSize.height);
            int G_height2 = static_cast<int>((hArray[1][i + 1] / static_cast<double>(G_maxVal)) * hSize.height);
            cv::line(histImage, cv::Point(i, hSize.height - G_height), cv::Point(i + 1, hSize.height - G_height2), cv::Scalar(0, 255, 0), 1);

            // R
            int R_height = static_cast<int>((hArray[2][i] / static_cast<double>(R_maxVal)) * hSize.height);
            int R_height2 = static_cast<int>((hArray[2][i + 1] / static_cast<double>(R_maxVal)) * hSize.height);
            cv::line(histImage, cv::Point(i, hSize.height - R_height), cv::Point(i + 1, hSize.height - R_height2), cv::Scalar(0, 0, 255), 1);
        }

        // ヒストグラム画像の表示
        cv::imshow(winname, histImage);
    }
}
