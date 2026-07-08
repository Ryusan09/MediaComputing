#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
    int nFile = 9;
    Mat points(nFile,1,CV_32FC3);
    std::vector<Mat> images;

    for (int i = 0; i < nFile; ++i)
    {
        //画像読み込み
        std::string fn = "flowers/flower";
        fn += std::to_string(i);
        fn += ".jpg";

        Mat img = imread(fn);
        if (!img.data)
        {
            std::cout << "No image data" << std::endl;
            return -1;
        }
        images.push_back(img);

        //(1)特徴ベクトル
        //bgrのそれぞれが画像中の何%を占めているかで分類
        float bSum = 0.0;
        float gSum = 0.0;
        float rSum = 0.0;
        for(int y = 0;y < img.rows;++y)
        {
            for(int x = 0;x < img.cols;++x)
            {
                //各画像のBGRの総和がいくつか
            }
        }
        float bgrSum = bSum+gSum+rSum;
        //k-meansへの入力pointsに
        //BGRのそれぞれがその画像の何%をしてているかを表す3要素のベクトルで渡す
        //Vec3f point(B,G,R);
        points.at<Vec3f>(i) = point;
    }

    //(3)k-meansを実行
    int k = 3;//クラスタの数
    Mat_<int> label(points.rows,1,CV_32SC1);//各画像が属するクラスタのインデックス
    Mat centers;//各クラスタの重心
    kmeans(points,k,label
            ,TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 100, 1.0)
            ,1,KMEANS_PP_CENTERS,centers);

    //(4)画像を表示
    for(int i = 0;i<nFile;++i)
    {
        int v = label(i);
        std::string l = "cluster";
        l += std::to_string(v);
        resize(images[i],images[i],Size(),0.1,0.1);
        putText(images[i],l,Point(20,40),FONT_HERSHEY_SIMPLEX
                ,1.5,Scalar(255,255,255),5);
    }

    imshow("dst1",images[0]);
    imshow("dst2",images[1]);
    imshow("dst3",images[2]);
    imshow("dst4",images[3]);
    imshow("dst5",images[4]);
    imshow("dst6",images[5]);
    imshow("dst7",images[6]);
    imshow("dst8",images[7]);
    imshow("dst9",images[8]);

    waitKey(0);
    return 0;
}
