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
    cv::Rect2d scanningArea;
    std::string lastScanned;
    while (cam.isOpened()) {
        cam.read(image);

        // Opens a window to select area to scan for barcodes
        if (scanningArea.empty()) {
            auto boundWindowName = "Bounding Box Selection";
            scanningArea = cv::selectROI(boundWindowName, image);
            cv::destroyWindow(boundWindowName);
        }

        // Gets a submatrix and searches for barcodes in that area
        auto subImage = image(scanningArea);
        auto codes = findBarCodes(scanner, subImage);

        // Draws area where barcodes are expected to be
        cv::rectangle(image, scanningArea, cv::Scalar(0, 255, 0), 3);

        // There should be only one barcode really
        if (codes.size() > 0) {
            lastScanned = codes.at(0).data;
        }
        // Displays last barcode found
        cv::putText(image, lastScanned, cv::Point(100, image.rows - 100), cv::FONT_HERSHEY_PLAIN, 20, cv::Scalar(0, 255, 0), 5);

        cv::imshow("Main", image);

        if (cv::waitKey(5) > 0) {
            break;
        }
    }

    return 0;
}
