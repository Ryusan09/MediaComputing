// g++ mc09-2.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>

// ソーベルフィルタでエッジ画像(8bit)を作成
static cv::Mat makeSobelEdge(const cv::Mat &binary)
{
  cv::Mat gx, gy, absGx, absGy, edge;
  cv::Sobel(binary, gx, CV_16S, 1, 0);
  cv::Sobel(binary, gy, CV_16S, 0, 1);
  cv::convertScaleAbs(gx, absGx);
  cv::convertScaleAbs(gy, absGy);
  cv::addWeighted(absGx, 0.5, absGy, 0.5, 0, edge);
  cv::threshold(edge, edge, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
  return edge;
}

int main(int argc, const char *argv[])
{
  // 画像読み込み
  cv::Mat sourceImage = cv::imread("images/aquarium.jpg");
  cv::Mat templateImage = cv::imread("images/fish.jpg");

  cv::Mat graySourceImage, grayTemplateImage;
  cv::Mat binarySourceImage, binaryTemplateImage;
  cv::Mat edgeSourceImage, edgeTemplateImage;
  cv::Mat distSourceImage;
  cv::Mat normalizedSourceImage;

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

  // 入力画像の距離変換画像を作成
  // 画像のグレースケール化&二値化
  cv::cvtColor(sourceImage, graySourceImage, cv::COLOR_BGR2GRAY);
  cv::threshold(graySourceImage, binarySourceImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

  // ソーベルフィルタでエッジ抽出
  edgeSourceImage = makeSobelEdge(binarySourceImage);

  // 白黒反転 (エッジを0、それ以外を255にして距離変換できるようにする)
  cv::bitwise_not(edgeSourceImage, edgeSourceImage);

  // 距離変換 (各画素から最も近いエッジまでの距離)
  cv::distanceTransform(edgeSourceImage, distSourceImage, cv::DIST_L2, 5);

  // 正規化 (0〜255)
  cv::normalize(distSourceImage, normalizedSourceImage, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);

  // テンプレート画像のエッジ画像を作成
  // 画像のグレースケール化&二値化
  cv::cvtColor(templateImage, grayTemplateImage, cv::COLOR_BGR2GRAY);
  cv::threshold(grayTemplateImage, binaryTemplateImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

  // ソーベルフィルタでエッジ抽出
  edgeTemplateImage = makeSobelEdge(binaryTemplateImage);

  // 距離変換画像とテンプレート画像のエッジ画像でマッチング
  // チャンファーマッチングはエッジ上の距離値の総和が最小となる位置が最良 → TM_CCORRの最小値
  cv::matchTemplate(normalizedSourceImage, edgeTemplateImage, resultImage, cv::TM_CCORR);

  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(resultImage, &minVal, &maxVal, &minLoc, &maxLoc);
  matchPoint = minLoc;
  val = minVal;
  printf("chamfer dissimilarity = %f at (%d, %d)\n", val, matchPoint.x, matchPoint.y);

  cv::Rect p = cv::Rect(matchPoint, templateImage.size());
  cv::rectangle(sourceImage, p, cv::Scalar(0, 0, 255), 3);

  cv::imshow("Source", sourceImage);
  cv::imshow("Template", templateImage);
  cv::imshow("EdgeSource", edgeSourceImage);
  cv::imshow("NormalizedSource", normalizedSourceImage);
  cv::imshow("EdgeTemplate", edgeTemplateImage);

  cv::waitKey(0);

  return 0;
}
