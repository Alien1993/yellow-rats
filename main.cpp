#include <algorithm>
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <zbar/Image.h>
#include <zbar/Symbol.h>

struct Barcode {
    std::string type;
    std::string data;
    std::vector<cv::Point> location;
};

// Find and decode barcodes and QR codes
static std::vector<Barcode> findBarCodes(cv::Mat& im)
{
    // Create zbar scanner
    zbar::ImageScanner scanner;

    // Configure scanner
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

    // Convert to grayscale
    cv::Mat imGray;
    cvtColor(im, imGray, cv::COLOR_BGR2GRAY);

    // Wrap image data in a zbar image
    zbar::Image image(im.cols, im.rows, "Y800", static_cast<uchar*>(imGray.data), im.cols * im.rows);

    // Scan the image for barcodes and QRCodes
    int n = scanner.scan(image);
    if (n == 0) {
        return {};
    }

    std::vector<Barcode> objects;
    // Gather results
    auto it = image.symbol_begin();
    while (it != image.symbol_end()) {
        Barcode obj;

        obj.type = it->get_type_name();
        obj.data = it->get_data();

        // Obtain location
        for (int i = 0; i < it->get_location_size(); i++) {
            obj.location.push_back(cv::Point(it->get_location_x(i), it->get_location_y(i)));
        }

        objects.push_back(obj);
        ++it;
    }
    return objects;
}

// Display barcode and QR code location
static void display(cv::Mat& im, std::vector<Barcode>& codes)
{
    // Loop over all decoded objects
    for (auto code : codes) {
        std::vector<cv::Point> points = code.location;
        std::vector<cv::Point> hull;

        // If the points do not form a quad, find convex hull
        if (points.size() > 4) {
            cv::convexHull(points, hull);
        } else {
            hull = points;
        }

        // Number of points in the convex hull
        int n = hull.size();

        for (int j = 0; j < n; j++) {
            cv::line(im, hull[j], hull[(j + 1) % n], cv::Scalar(255, 0, 0), 3);
        }
        std::cout << code.data << std::endl;
    }

    // Display results
    cv::imshow("Results", im);
}

int main(int argc, char** argv)
{
    cv::VideoCapture cam;
    // Gets the first cam it finds
    cam.open(0, cv::CAP_ANY);
    cv::Mat image;
    while (cam.isOpened()) {
        cam.read(image);
        auto codes = findBarCodes(image);

        display(image, codes);

        if (cv::waitKey(5) > 0) {
            break;
        }
    }

    return 0;
}
