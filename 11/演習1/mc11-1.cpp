#include <opencv2/opencv.hpp>

using namespace cv;
int main()
{
    int k = 4;//クラスタの数
    Mat blank = Mat(500,500,CV_8UC3);
    Mat centers;
    Mat labels;
    ////kを増やした場合には色も増やしてください
    Scalar colors[] =
            {
                Scalar (255,0,0),
                Scalar (0,255,0),
                Scalar (0,0,255),
                Scalar (0,255,255),
            };

    if(k > sizeof(colors))
    {
        std::cout << "Not enough color" << std::endl;
        return -1;
    }

    //2次元座標上に点を用意
    int nPoint = 12;
    Mat points(nPoint,1,CV_32FC2);

    points.at<Point2f>(0) = Point2f(20,170);
    points.at<Point2f>(1) = Point2f(450,450);
    points.at<Point2f>(2) = Point2f(200,450);
    points.at<Point2f>(3) = Point2f(300,10);
    points.at<Point2f>(4) = Point2f(450,150);
    points.at<Point2f>(5) = Point2f(450,350);
    points.at<Point2f>(6) = Point2f(42,319);
    points.at<Point2f>(7) = Point2f(200,420);
    points.at<Point2f>(8) = Point2f(150,150);
    points.at<Point2f>(9) = Point2f(350,350);
    points.at<Point2f>(10) = Point2f(298,212);
    points.at<Point2f>(11) = Point2f(100,200);

    //k-meansを実行
    kmeans(points,k,labels,
            TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,10,1.0),
            1,KMEANS_RANDOM_CENTERS,centers);

    //blankに各点をプロット(円)
    for(int i = 0;i<points.rows;++i)
    {
        int label = labels.at<int>(i);
        Point point = points.at<Point2f>(i);
        circle(blank,point,10,colors[label],-1,4);
    }

    //blankに各クラスタの重心をプロット(四角)
    for(int label = 0;label < centers.rows;++label)
    {
        Point center = centers.at<Point2f>(label);
        Point p1 = Point2f(center.x -10,center.y -10);
        Point p2 = Point2f(center.x +10,center.y +10);
        rectangle(blank,p1,p2,colors[label],1,4);
    }

    //画像の表示
    imshow("k-means",blank);

    waitKey(0);
    return 0;
}
