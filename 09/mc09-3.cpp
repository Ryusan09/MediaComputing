// 課題: 動画でパターンマッチングしよう (処理結果の動画を提出)
// g++ mc09-3.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>

int main(int argc, const char *argv[])
{
  // 動画とテンプレート画像の読み込み
  cv::VideoCapture capture("images/game.mp4");
  cv::Mat templateImage = cv::imread("images/player.png");

  if (!capture.isOpened() || templateImage.empty())
  {
    printf("File not found\n");
    exit(0);
  }

  // 動画の情報を取得
  int width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
  int height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);
  double fps = capture.get(cv::CAP_PROP_FPS);

  // 処理結果を書き出す動画ファイル
  cv::VideoWriter writer("result.mp4",
                         cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                         fps, cv::Size(width, height));
  if (!writer.isOpened())
  {
    printf("Failed to open VideoWriter\n");
    exit(0);
  }

  // テンプレートマッチングの設定
  int method = cv::TM_CCOEFF_NORMED; // ZNCC(明るさ変化に強い)
  cv::Mat frame, resultImage;

  while (true)
  {
    // 1フレーム取得
    capture >> frame;
    if (frame.empty())
      break;

    // テンプレートマッチング
    cv::matchTemplate(frame, templateImage, resultImage, method);

    // 最も類似度の高い位置を取得 (ZNCCなので最大値)
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(resultImage, &minVal, &maxVal, &minLoc, &maxLoc);

    // 検出位置に矩形を描画
    cv::Rect p(maxLoc, templateImage.size());
    cv::rectangle(frame, p, cv::Scalar(0, 0, 255), 3);

    // 処理結果を動画に書き出し
    writer << frame;
  }

  capture.release();
  writer.release();
  printf("Saved: result.mp4\n");

  return 0;
}
