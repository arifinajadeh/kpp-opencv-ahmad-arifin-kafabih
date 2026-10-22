
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    cv::Mat frame, resized_frame, gray;

    cv::Size papancatur(8, 6);
    double lebarKotak = 23.0;

    vector<cv::Point3f> objectPoints;

    for (int i = 0; i < papancatur.height; i++) {
        for (int j = 0; j < papancatur.width; j++) {
            objectPoints.push_back(
                cv::Point3f(j * lebarKotak, i * lebarKotak, 0)
            );
        }
    }

    vector<vector<cv::Point3f>> allObjectPoints;
    vector<vector<cv::Point2f>> allImagePoints;
    vector<cv::Point2f> corners;

    cv::VideoCapture cap(2);

    if (!cap.isOpened()) {
        cerr << "Kamera gagal dibuka!" << endl;
        return -1;
    }

    cv::Mat K, distCoeffs;
    vector<cv::Mat> rvecs, tvecs;
    bool calibrated = false;

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            cerr << "Error: frame kosong!" << endl;
            break;
        }

        cv::resize(frame, resized_frame, cv::Size(640, 480));
        cv::cvtColor(resized_frame, gray, cv::COLOR_BGR2GRAY);

        bool found = cv::findChessboardCorners(
            gray, papancatur, corners
        );

        if (found) {
            cv::drawChessboardCorners(
                resized_frame, papancatur, corners, found
            );
        }

        cv::imshow("Chessboard Detection", resized_frame);

        char key = (char)cv::waitKey(25);

        if (key == 's' || key == 'S') {
            if (found) {
                allObjectPoints.push_back(objectPoints);
                allImagePoints.push_back(corners);
                
                cout << "Sampel tersimpan: "
                     << allImagePoints.size() << endl;
            } else {
                cout << "Chessboard belum terdeteksi!" << endl;
            }
        }

        if (key == 'c' || key == 'C') {
            if (allImagePoints.size() < 10) {
                cout << "minimal 10 sampel dahulu"
                     << endl;
            } else {

                K = cv::Mat::eye(3, 3, CV_64F);
                distCoeffs = cv::Mat::zeros(5, 1, CV_64F);

                double rms = cv::calibrateCamera(   
                    allObjectPoints,
                    allImagePoints,
                    resized_frame.size(),
                    K,
                    distCoeffs,
                    rvecs,
                    tvecs
                );

                cout << "\nRMS: " << rms << endl;
                cout << "Camera Matrix:\n"
                     << K << endl;
                cout << "Distortion Coefficients:\n"
                     << distCoeffs << endl;

                calibrated = true;
            }
        }

        if (key == 'q') {
            break;
        }
    }
    cv::FileStorage fs ("../yaml/kalibrasi.yaml", cv::FileStorage::WRITE);
    fs << "K" << K;
    fs.release();
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
