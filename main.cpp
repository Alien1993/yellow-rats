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

// Uses a configured scanner to search for barcodes in im
static std::vector<Barcode> findBarCodes(zbar::ImageScanner& scanner, cv::Mat& image)
{
    // Convert to grayscale
    cv::Mat imageGray;
    cv::cvtColor(image, imageGray, cv::COLOR_BGR2GRAY);

    // Wrap image data in a zbar image
    zbar::Image zImage(image.cols, image.rows, "Y800", static_cast<uchar*>(imageGray.data), image.cols * image.rows);

    // Scan the image for barcodes and QRCodes
    int n = scanner.scan(zImage);
    if (n == 0) {
        return {};
    }

    std::vector<Barcode> objects;
    // Gather results
    auto it = zImage.symbol_begin();
    while (it != zImage.symbol_end()) {
        Barcode obj;

        obj.type = it->get_type_name();
        obj.data = it->get_data();

        // Obtain location
        for (int i = 0; i < it->get_location_size(); i++) {
            obj.location.push_back(cv::Point(it->get_location_x(i), it->get_location_y(i)));
        }

        objects.push_back(obj);
        std::cout << "====== START ======\n";
        std::cout << "Data: " << it->get_data() << '\n';
        std::cout << "Type: " << it->get_type_name() << '\n';
        std::cout << "======= END =======\n";
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
    }

    // Display results
    cv::imshow("Main", im);
}

static int setScannerSettings(zbar::ImageScanner& scanner)
{
    return scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0) + scanner.set_config(zbar::ZBAR_EAN5, zbar::ZBAR_CFG_ENABLE, 1);
}

int main(int argc, char** argv)
{
    zbar::ImageScanner scanner;
    if (setScannerSettings(scanner) != 0) {
        std::cerr << "Error setting ImageScanner config\n";
        return 1;
    }

    cv::namedWindow("Main");
    cv::VideoCapture cam;

    // Gets the first cam it finds
    cam.open(0, cv::CAP_ANY);
    cv::Mat image;
    while (cam.isOpened()) {
        cam.read(image);
        auto codes = findBarCodes(scanner, image);
        display(image, codes);

        if (cv::waitKey(5) > 0) {
            break;
        }
    }

    return 0;
}
