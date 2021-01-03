#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <zbar/Image.h>
#include <zbar/Symbol.h>

#include "comic.h"

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
    std::string outputFilename("output.csv");
    if (argc > 1) {
        outputFilename = std::string(argv[1]);
    }

    std::ofstream outputFile(outputFilename, std::ios::app);

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
    cam.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cam.set(cv::CAP_PROP_FPS, 30.0);
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
        cv::rectangle(image, scanningArea, cv::Scalar(255, 0, 255), 3);

        // There should be only one barcode really
        if (codes.size() > 0) {
            lastScanned = codes.at(0).data;
        }

        if (!lastScanned.empty()) {
            // Displays last barcode found
            cv::putText(image, lastScanned, cv::Point(100, image.rows - 100), cv::FONT_HERSHEY_PLAIN, 20, cv::Scalar(0, 0, 255), 5);

            cv::imshow("Main", image);

            // We're scanning EAN-5 but we don't need the first digit since only the last four are used to store the comic number
            unsigned int number = std::atoi(lastScanned.substr(1).data());

            auto quality = Comic::NONE;
            // Waits for number to be pressed to set quality
            switch (cv::waitKey()) {
            case 49:
            case 177:
                // 1 / Mint
                quality = Comic::MINT;
                break;
            case 50:
            case 178:
                // 2 / Near Mint
                quality = Comic::NEAR_MINT;
                break;
            case 51:
            case 179:
                // 3 / Very Fine
                quality = Comic::VERY_FINE;
                break;
            case 52:
            case 180:
                // 4 / Fine
                quality = Comic::FINE;
                break;
            case 53:
            case 181:
                // 5 / Very Good
                quality = Comic::VERY_GOOD;
                break;
            case 54:
            case 182:
                // 6 / Good
                quality = Comic::GOOD;
                break;
            case 55:
            case 183:
                // 7 / Fair
                quality = Comic::FAIR;
                break;
            case 56:
            case 184:
                // 8 / Poor
                quality = Comic::POOR;
                break;
            default:
                break;
            }
            Comic c { number, quality };
            lastScanned = "";

            outputFile << c.number << ',' << c.qualityString() << '\n';
        }

        cv::imshow("Main", image);

        // Exits when pressing ESC
        if (cv::waitKey(1) == 27) {
            outputFile.close();
            break;
        }
    }

    return 0;
}
