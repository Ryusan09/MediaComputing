//g++ mc11-4.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace cv::ml;

int main(int, char**)
{
  std::vector<Mat> trainImage(4); //学習用画像
  trainImage[0] = imread("trainImage/sweets1.jpg", IMREAD_COLOR);
  if (trainImage[0].data == 0) {                       //画像ファイルが読み込めなかった場合
      printf("File not found\n");
      exit(0);
  }

  trainImage[1] = imread("trainImage/sweets2.jpg", IMREAD_COLOR);
  if (trainImage[1].data == 0) {                       //画像ファイルが読み込めなかった場合
      printf("File not found\n");
      exit(0);
  }

  trainImage[2] = imread("trainImage/town1.jpg", IMREAD_COLOR);
  if (trainImage[2].data == 0) {                       //画像ファイルが読み込めなかった場合
      printf("File not found\n");
      exit(0);
  }

  trainImage[3] = imread("trainImage/town2.jpg", IMREAD_COLOR);
  if (trainImage[3].data == 0) {                       //画像ファイルが読み込めなかった場合
      printf("File not found\n");
      exit(0);
  }

    std::vector<Mat> testImage(2); //テスト用画像

    testImage[0] = imread("testImage/town.jpg", IMREAD_COLOR);
    if (testImage[0].data == 0) {                       //画像ファイルが読み込めなかった場合
        printf("File not found\n");
        exit(0);
    }

    testImage[1] = imread("testImage/sweets.jpg", IMREAD_COLOR);
    if (testImage[1].data == 0) {                       //画像ファイルが読み込めなかった場合
        printf("File not found\n");
        exit(0);
    }

    int labels[4] = {1, 1, -1, -1}; //学習用画像のラベル

    Mat labelsMat(4, 1, CV_32SC1, labels); //ラベルの行列

    HOGDescriptor hog;

    std::vector<std::vector<float> > trainHOG; //学習用画像のHOG特徴量
    std::vector<std::vector<float> > testHOG; //テスト用画像のHOG特徴量

    //特徴量計算(学習用画像)
    for(int i = 0; i < trainImage.size(); i++){
      std::vector<Point> locs;
      std::vector<float> descriptors;
      hog.compute(trainImage[i], descriptors, Size(32, 32), Size(0, 0), locs);
      trainHOG.push_back(descriptors);
    }

    int descriptor_size = trainHOG[0].size();

    Mat trainMat(trainHOG.size(), descriptor_size, CV_32FC1);

    for(int i = 0; i < (int)trainHOG.size(); i++){
        for(int j = 0;j<descriptor_size;j++){
            trainMat.at<float>(i,j) = trainHOG[i][j];
        }
    }

    //特徴量計算(テスト画像)
    for(int i = 0; i < testImage.size(); i++){
      std::vector<Point> locs;
      std::vector<float> descriptors;
      hog.compute(testImage[i], descriptors, Size(32, 32), Size(0, 0), locs);
      testHOG.push_back(descriptors);
    }

    descriptor_size = testHOG[0].size();

    Mat testMat(testHOG.size(), descriptor_size, CV_32FC1);

    for(int i = 0; i < (int)testHOG.size(); i++) {
        for(int j = 0;j<descriptor_size;j++){
            testMat.at<float>(i,j) = testHOG[i][j];
        }
    }

    //svm
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

    //学習
    svm->trainAuto(trainMat, ROW_SAMPLE, labelsMat);

    Mat results;

    //識別
    svm->predict(testMat, results);

    for(int i = 0; i < results.rows; i++){
      if(results.at<float>(i, 0) == 1){
        std::printf("%d:sweets\n", i);
      }else{
        std::printf("%d:town\n", i);
      }
    }

    imshow("0", testImage[0]);
    imshow("1", testImage[1]);

    waitKey();
    return 0;
}
