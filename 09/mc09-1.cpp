// g++ mc09-1.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>

int main(int argc, const char *argv[])
{
  // 画像読み込み
  cv::Mat sourceImage = cv::imread("images/find_wally.jpg");
  cv::Mat templateImage = cv::imread("images/wally.png");

  if (sourceImage.empty() || templateImage.empty())
  {
    printf("File not found\n");
    exit(0);
  }

  // 出力画像
  cv::Mat resultImage(cv::Size(sourceImage.cols, sourceImage.rows), CV_8UC3);
  cv::Point matchPoint;
  cv::Rect resultRect(0, 0, templateImage.cols, templateImage.rows);
  double val;

  // テンプレートマッチング
  // method: SSD → cv::TM_SQDIFF_NORMED / NCC → cv::TM_CCORR_NORMED / ZNCC → cv::TM_CCOEFF_NORMED
  int method = cv::TM_CCOEFF_NORMED; // ZNCC
  cv::matchTemplate(sourceImage, templateImage, resultImage, method);

  // 最小値・最大値とその位置を取得
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(resultImage, &minVal, &maxVal, &minLoc, &maxLoc);

  // マッチング位置の指定
  // SSD(相違度)は最小値の位置、NCC/ZNCC(類似度)は最大値の位置が最良
  if (method == cv::TM_SQDIFF || method == cv::TM_SQDIFF_NORMED) {
    matchPoint = minLoc;
    val = minVal;
  } else {
    matchPoint = maxLoc;
    val = maxVal;
  }
  printf("match score = %f at (%d, %d)\n", val, matchPoint.x, matchPoint.y);
  cv::Rect p = cv::Rect(matchPoint, templateImage.size());

  // 矩形の描画
  cv::rectangle(sourceImage, p, cv::Scalar(255, 0, 0), 3);

  cv::imshow("Source", sourceImage);
  cv::imshow("Template", templateImage);
  cv::imshow("Result", resultImage);

  cv::waitKey(0);

  return 0;
}
