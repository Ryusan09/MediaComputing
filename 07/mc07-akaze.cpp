//g++ mc07-akaze.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
#include <opencv2/features2d.hpp>

int main(int argc, const char* argv[]) {
    // 画像の読み込み
    cv::Mat Source1 = cv::imread("images/panorama1.jpg", cv::IMREAD_COLOR);
    if (Source1.empty()) {
        std::cout << "Failed to open Source image1" << std::endl;
        return -1;
    }

    cv::Mat Source2 = cv::imread("images/panorama2.jpg", cv::IMREAD_COLOR);
    if (Source2.empty()) {
        std::cout << "Failed to open Source image2" << std::endl;
        return -1;
    }

//    cv::Mat Source3 = cv::imread("images/panorama3.jpg", cv::IMREAD_COLOR);
//    if (Source3.empty()) {
//        std::cout << "Failed to open Source image3" << std::endl;
//        return -1;
//    }

    // 画像を配列に格納
    std::vector<cv::Mat> Sources;
    Sources.push_back(Source1);
    Sources.push_back(Source2);
//    Sources.push_back(Source3);

    cv::imshow("Source 1", Source1);
    cv::imshow("Source 2", Source2);

    // A-KAZE検出器の生成
    cv::Ptr<cv::Feature2D> akaze = cv::AKAZE::create();

    // 画像を読み込み、特徴点を計算
    std::vector<cv::KeyPoint> kp1, kp2;
    cv::Mat des1, des2;
    akaze->detectAndCompute(Sources[0], cv::noArray(), kp1, des1);
    akaze->detectAndCompute(Sources[1], cv::noArray(), kp2, des2);


    // 特徴点を描画した画像を作成
    cv::Mat img1_with_keypoints, img2_with_keypoints;
    cv::drawKeypoints(Sources[0], kp1, img1_with_keypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::drawKeypoints(Sources[1], kp2, img2_with_keypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    // 画像を表示
     cv::imshow("Image 1 with Keypoints", img1_with_keypoints);
     cv::imshow("Image 2 with Keypoints", img2_with_keypoints);

    // 特徴点のマッチングを行い、マッチング結果を描画
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> nn_matches;
    matcher.knnMatch(des2, des1, nn_matches, 2); // テンプレート画像が第一引数なので注意

    float ratio = 0.5f;
    std::vector<cv::DMatch> good_matches;
    for (size_t i = 0; i < nn_matches.size(); i++) {
        if (nn_matches[i][0].distance < ratio * nn_matches[i][1].distance) {
            good_matches.push_back(nn_matches[i][0]);
        }
    }

    cv::Mat img_matches;
    cv::drawMatches(Sources[1], kp2, Sources[0], kp1, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // マッチング結果を表示
    cv::namedWindow("Matches", cv::WINDOW_NORMAL);
     cv::imshow("Matches", img_matches);
    
    // 対応点抽出
    std::vector<cv::Point2f> match_point[2];
    for (int i = 0; i < good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        match_point[0].push_back(kp2[good_matches[i].queryIdx].pt);
        match_point[1].push_back(kp1[good_matches[i].trainIdx].pt);
    }
    
    //ホモグラフィ行列推定(RANSAC)
    cv::Mat masks;
    cv::Mat Homo = cv::findHomography(match_point[0], match_point[1], masks, cv::RANSAC, 3);
    
    //画像の変形
    cv::Mat resultImage;
    cv::warpPerspective(Sources[1], resultImage, Homo, cv::Size(static_cast<int>(Sources[1].cols * 1.5), static_cast<int>(Sources[1].rows * 1.1)));
    for (int y = 0; y < Sources[0].rows; y++){
        for (int x = 0; x < Sources[0].cols; x++){
            resultImage.at<cv::Vec3b>(y, x) = Sources[0].at<cv::Vec3b>(y, x);
        }
    }
    cv::imshow("Stitching", resultImage);

    cv::waitKey(0);

    return 0;
}
