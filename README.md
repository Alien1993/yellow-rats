# Yellow Rats

This is a small thing I made to scan and catalog my comics.

## Usage

The webcam used is the first available found.
When started you must select a bounding box, press `Enter` to confirm your selection.
The barcodes must be inside that box to be found, those outside are ignored.

Only EAN-5 barcodes are supported since usually are the ones used to store the release number.

Once a code is found the execution stops and the scanned value is printed.
You must press a number to set the quality of the comic or `Esc` to skip it:

1. MINT
1. NEAR MINT
1. VERY FINE
1. FINE
1. VERY GOOD
1. GOOD
1. FAIR
1. POOR

If the number has already been scanned the comic is skipped automatically and a message is shown.

The list is saved to `output.csv` by default, to use a different file specify it as argument when starting the program:

```
./yellow-rats my-file.csv
```

To exit press `Esc`.

## Requirements

- C++14 compliant compiler
- OpenCV 4.0.1
- CMake 2.8
- Zbar 0.23

## Build

```
mkdir build
cd build
cmake ..
cmake --build .
```
