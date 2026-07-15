// g++ -std=c++11 mc12-1.cpp `pkg-config --cflags --libs opencv4`
// https://www.qoosky.io/techs/15ea5b1dac
#include <opencv2/opencv.hpp>
#include <iostream>

void drawAxis(cv::Mat& img, cv::Point p, cv::Point q, cv::Scalar color, const float scale = 0.2) {
    //矢印の描画 drawAxis(描画先画像, 矢印元の座標, 矢印先の座標, 線の色, 線の長さの拡大率)
    double angle = atan2( (double) p.y - q.y, (double) p.x - q.x ); // angle in radians
    double hypotenuse = sqrt( (double) (p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
    // Here we lengthen the arrow by a factor of scale
    q.x = (int) (p.x - scale * hypotenuse * cos(angle));
    q.y = (int) (p.y - scale * hypotenuse * sin(angle));
    cv::line(img, p, q, color, 1, cv::LINE_AA);
    // create the arrow hooks
    p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
    cv::line(img, p, q, color, 1, cv::LINE_AA);
    p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
    cv::line(img, p, q, color, 1, cv::LINE_AA);
}

void pca(const std::vector<cv::Point> &pts, cv::Mat &img) {
    // 主成分分析のために、対象となる輪郭のみを別のバッファにコピー
    cv::Mat data_pts = cv::Mat(pts.size(), 2, CV_64F); // [pts.size() x 2] 行列
    for (int i = 0; i < data_pts.rows; i++) {
        data_pts.at<double>(i, 0) = pts[i].x;
        data_pts.at<double>(i, 1) = pts[i].y;
    }

    // 主成分分析の実行

}

int main() {
    cv::Mat srcImg = cv::imread("images/pca.jpg", -1);
    if(srcImg.empty()) {
        printf("Image file not found\n");
        exit(0);
    }
    cv::Mat resultImg = srcImg.clone();

    // グレースケール画像に変換
    cv::Mat gray;
    cv::cvtColor(srcImg, gray, cv::COLOR_BGR2GRAY);

    // 50 を閾値として二値化
    cv::Mat bw;
    cv::threshold(gray, bw, 50, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::imshow("gray", gray);
    cv::imshow("bw", bw);

    // 輪郭検出
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(bw, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    for (size_t i = 0; i < contours.size(); i++) {
        // 面積が小さすぎる場合と多きすぎる場合を除外
        double area = cv::contourArea(contours[i]);
        if (area < 1e2 || 1e5 < area) {
            continue;
        }
        // 輪郭描画
        cv::drawContours(resultImg, contours, i, cv::Scalar(0, 0, 255), 2);
        // 各輪郭について主成分分析
        pca(contours[i], resultImg);
    }

    cv::imshow("output", resultImg);
    cv::waitKey(0);
    cv::imwrite("images/pcaResult.jpg", resultImg);
    return 0;
}
