#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Missing image file argument" << std::endl;
        return -1;
    }

    cv::Mat original = cv::imread(argv[1]);
    cv::Mat image;

    // Converts in memory loaded image from BGR to HSV
    cv::cvtColor(original, image, cv::COLOR_BGR2HSV);

    auto lowerBound = cv::Scalar(0, 0, 0);
    auto upperBound = cv::Scalar(50, 255, 150);

    cv::inRange(image, lowerBound, upperBound, image);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(image, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    //    cv::findContours(image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_L1);

    //    for (int i = 0; i >= 0; i = hierarchy[i][0]) {
    //        cv::drawContours(original, contours, i, cv::Scalar(0, 0, 255), cv::LINE_4, 1, hierarchy);
    //    }

    //    std::vector<cv::Point> approx;
    //    std::vector<std::vector<cv::Point>> squares;
    //    for (int i = 0; i < contours.size(); i++) {
    //        cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * 0.02, true);
    //        if (/*approx.size() == 4 && */ std::fabs(cv::contourArea(approx) > 1000) && cv::isContourConvex(approx)) {
    //            double maxCosine = 0;
    //            //            for (int j = 2; j < 5; j++) {
    //            //                double cosine = std::fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
    //            //                maxCosine = std::max(maxCosine, cosine);
    //            //            }

    //            if (maxCosine < 0.4) {
    //                squares.push_back(approx);
    //            }
    //        }
    //    }

    //    for (int i = 0; i < squares.size(); i++) {
    //        cv::polylines(original, squares[i], true, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    //    }

    cv::resize(image, image, cv::Size(image.cols, image.rows) / 3);
    cv::imshow("Converted", image);

    cv::resize(original, original, cv::Size(original.cols, original.rows) / 3);
    cv::imshow("Original", original);

    cv::waitKey(0);

    return 0;
}
