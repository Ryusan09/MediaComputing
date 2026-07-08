// g++ mc11-kadai.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace cv::ml;

namespace
{
const int SWEETS_LABEL = 1;
const int DRINK_LABEL = -1;
const Size HOG_SIZE(64, 64);

struct ImageSample
{
    std::string path;
    int label;
};

std::string labelName(float label)
{
    return label == SWEETS_LABEL ? "sweets" : "drink";
}

Mat loadNormalizedImage(const std::string& path)
{
    Mat image = imread(path, IMREAD_COLOR);
    if (image.empty())
    {
        std::cerr << "File not found: " << path << std::endl;
        exit(1);
    }

    Mat resized;
    resize(image, resized, HOG_SIZE);
    return resized;
}

std::vector<float> computeHOG(const Mat& image, HOGDescriptor& hog)
{
    std::vector<float> descriptor;
    std::vector<Point> locations;
    hog.compute(image, descriptor, Size(8, 8), Size(0, 0), locations);
    return descriptor;
}

Mat createFeatureMat(const std::vector<ImageSample>& samples, HOGDescriptor& hog)
{
    std::vector<std::vector<float> > descriptors;
    for (size_t i = 0; i < samples.size(); ++i)
    {
        Mat image = loadNormalizedImage(samples[i].path);
        descriptors.push_back(computeHOG(image, hog));
    }

    Mat featureMat(static_cast<int>(descriptors.size()),
                   static_cast<int>(descriptors[0].size()),
                   CV_32FC1);

    for (int y = 0; y < featureMat.rows; ++y)
    {
        for (int x = 0; x < featureMat.cols; ++x)
        {
            featureMat.at<float>(y, x) = descriptors[y][x];
        }
    }

    return featureMat;
}

Mat createLabelMat(const std::vector<ImageSample>& samples)
{
    Mat labels(static_cast<int>(samples.size()), 1, CV_32SC1);
    for (int i = 0; i < labels.rows; ++i)
    {
        labels.at<int>(i, 0) = samples[i].label;
    }
    return labels;
}

void appendResultImage(Mat& canvas, const std::string& path, const std::string& label)
{
    Mat image = imread(path, IMREAD_COLOR);
    if (image.empty())
    {
        std::cerr << "File not found: " << path << std::endl;
        exit(1);
    }

    Mat tile;
    resize(image, tile, Size(180, 180));
    copyMakeBorder(tile, tile, 28, 12, 12, 12, BORDER_CONSTANT, Scalar(40, 40, 40));
    putText(tile, label, Point(16, 24), FONT_HERSHEY_SIMPLEX, 0.75,
            Scalar(255, 255, 255), 2);

    if (canvas.empty())
    {
        canvas = tile;
        return;
    }

    hconcat(canvas, tile, canvas);
}
}

int main(int argc, char** argv)
{
    bool showWindow = argc > 1 && std::string(argv[1]) == "--show";

    std::vector<ImageSample> trainSamples;
    trainSamples.push_back(ImageSample{"trainImage/sweets1.JPG", SWEETS_LABEL});
    trainSamples.push_back(ImageSample{"trainImage/sweets2.JPG", SWEETS_LABEL});
    trainSamples.push_back(ImageSample{"trainImage/coffee2.JPG", DRINK_LABEL});
    trainSamples.push_back(ImageSample{"trainImage/drink1.jpg", DRINK_LABEL});

    std::vector<ImageSample> testSamples;
    testSamples.push_back(ImageSample{"testImage/sweets.jpg", 0});
    testSamples.push_back(ImageSample{"testImage/drink.jpg", 0});

    HOGDescriptor hog(HOG_SIZE, Size(16, 16), Size(8, 8), Size(8, 8), 9);
    Mat trainMat = createFeatureMat(trainSamples, hog);
    Mat labelsMat = createLabelMat(trainSamples);
    Mat testMat = createFeatureMat(testSamples, hog);

    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 1e-6));
    svm->train(trainMat, ROW_SAMPLE, labelsMat);

    Mat results;
    svm->predict(testMat, results);

    Mat canvas;
    for (int i = 0; i < results.rows; ++i)
    {
        std::string predicted = labelName(results.at<float>(i, 0));
        std::cout << i << ":" << predicted << " (" << testSamples[i].path << ")" << std::endl;
        appendResultImage(canvas, testSamples[i].path, predicted);
    }

    imwrite("result_kadai.jpg", canvas);

    if (showWindow)
    {
        imshow("result", canvas);
        waitKey(0);
    }

    return 0;
}
