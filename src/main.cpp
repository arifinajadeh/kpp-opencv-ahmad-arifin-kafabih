
#include <opencv2/opencv.hpp>
#include "tracker.hpp"
#include <iostream>
// #include <vector>
// #include <string>
// #include <algorithm>


using namespace std;
int main() {
    cv::VideoCapture cap(2);

    if (!cap.isOpened()) {
        cerr << "Error: Kamera gagal dibuka!" << endl;
        return -1;
    }

    cv::Mat K;  

    cv::FileStorage fs(
        "../yaml/kalibrasi.yaml",
        cv::FileStorage::READ
    );

    if (!fs.isOpened()) {
        cerr << "Error: File kalibrasi.yaml gagal dibuka!"
             << endl;
        return -1;
    }

    fs["K"] >> K;
    fs.release();

    if (K.empty() || K.rows != 3 || K.cols != 3) {
            cerr << "Error: Matriks K tidak valid!" << endl;
            return -1;
    }

    if (K.type() != CV_64F) {
        K.convertTo(K, CV_64F);
    }

    double fx = K.at<double>(0, 0);
    double fy = K.at<double>(1, 1);
    double cx = K.at<double>(0, 2);
    double cy = K.at<double>(1, 2);
    Tracker tracker(fx, fy, cx, cy);

    cv::Mat frame, hsl_video;
    cv::Mat blueMask, redMask, yellowMask, greenMask;
    cv::Mat redMask1, redMask2;
    cv::Mat frame_result;

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            cerr << "Error: Frame kamera kosong!" << endl;
            break;
        }

        // BGR ke HLS
        cv::cvtColor(
            frame,
            hsl_video,
            cv::COLOR_BGR2HLS
        );

        // Mask biru
        cv::inRange(
            hsl_video,  
            cv::Scalar(85, 25, 50),
            cv::Scalar(125, 200, 255),
            blueMask
        );
         //mask kuning 
        cv::inRange(hsl_video, 
            cv::Scalar(20, 50, 50), 
            cv::Scalar(50, 230, 255), 
        yellowMask);
        
        //mask hijau
        cv::inRange(
            hsl_video,
            cv::Scalar(75, 35, 60),
            cv::Scalar(100, 255, 255),
            greenMask
        );
        // cv::inRange(
        //     hsl_video,
        //     cv::Scalar(0, 50, 50),
        //     cv::Scalar(10, 255, 255),
        //     redMask1
        // );

        // cv::inRange(
        //     hsl_video,
        //     cv::Scalar(170, 50, 50),
        //     cv::Scalar(180, 255, 255),
        //     redMask2
        // );

        // cv::bitwise_or(
        //     redMask1,
        //     redMask2,
        //     redMask
        // );
        frame_result = frame.clone();

        deteksiWarna(
            blueMask, frame_result, "BIRU",
            cv::Scalar(255, 0, 0), 700.0
        );

        deteksiWarna(
            yellowMask, frame_result, "KUNING",
            cv::Scalar(0, 255, 255), 750.0
        );

         deteksiWarna(
            greenMask, frame_result, "HIJAU",
            cv::Scalar(0, 255, 0), 550.0
        );
        
        cv::imshow("Result", frame_result);
        cv::imshow("Mask Biru", blueMask);
        cv::imshow("Mask yellow", yellowMask);
        cv::imshow("Mask Green", greenMask);

        if (cv::waitKey(1) == 'q') {    
            break;
        }
    }       
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
