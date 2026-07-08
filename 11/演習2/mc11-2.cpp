#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
    //(1)入力
    Mat src = imread("sunset.jpg");
    if (!src.data)
    {
        std::cout << "NO image data" << std::endl;
        return -1;
    }

    //(2)画像を，1行の行列となるように変形
    Mat points;
    src.convertTo(points, CV_32FC3);
    points = points.reshape(0, src.rows * src.cols);

    //(3)k-means
    int k = 10;//クラスタの数
    Mat_<int> label(points.size(),CV_32SC1);//各画素が属するクラスタのインデックス
    Mat centers;//各クラスタの重心
    kmeans(points, k, label
            , TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 1.0)
            , 1,KMEANS_PP_CENTERS, centers);

    //(4)各ピクセル値を，属するクラスタの中心値で置き換え
    Mat dst(src.size(),src.type());
    MatIterator_<Vec3f> itf = centers.begin<Vec3f>();
    MatIterator_<Vec3b> itd = dst.begin<Vec3b>(), itd_end = dst.end<Vec3b>();
    for(int i = 0; itd != itd_end; ++itd, ++i)
    {
        Vec3f color = itf[label(1,i)];
        (*itd)[0] = saturate_cast<uchar>(color[0]);
        (*itd)[1] = saturate_cast<uchar>(color[1]);
        (*itd)[2] = saturate_cast<uchar>(color[2]);
    }
    
    imshow("dst",dst);
    waitKey(0);

    return 0;
}
