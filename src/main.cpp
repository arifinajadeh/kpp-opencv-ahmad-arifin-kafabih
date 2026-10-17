
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Fungsi untuk mendeteksi satu warna
void deteksiWarna(
    const cv::Mat& mask,
    cv::Mat& frame_result,
    const string& namaWarna,
    cv::Scalar warnaKotak,
    double fx, double fy, double cx, double cy,
    double lebarAsli
) {
    cv::Mat morphImage;

    
    cv::erode(
        mask, morphImage, cv::Mat(),
        cv::Point(-1, -1), 1
    );

    cv::dilate(
        morphImage, morphImage, cv::Mat(),
        cv::Point(-1, -1), 2
    );

    vector<vector<cv::Point>> contours;
    cv::findContours(
        morphImage.clone(),
        contours,
        cv::RETR_EXTERNAL,
        cv::CHAIN_APPROX_SIMPLE
    );

    for (size_t i = 0; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) <= 1000) {
            continue;
        }

        cv::Rect boundingBox = cv::boundingRect(contours[i]);

        double wpixel = boundingBox.width;

        cv::Moments m = cv::moments(contours[i]);

        if (m.m00 == 0 || wpixel <= 0) {
            continue;
        }

        double u = m.m10 / m.m00;
        double v = m.m01 / m.m00;

        double Z = (fx * lebarAsli) / wpixel;
        double X = ((u - cx) * Z) / fx;
        double Y = ((v - cy) * Z) / fy;

        cv::rectangle(
            frame_result,
            boundingBox,
            warnaKotak,
            2
        );

        cv::circle(
            frame_result,
            cv::Point(
                static_cast<int>(u),
                static_cast<int>(v)
            ),
            5,
            cv::Scalar(0, 0, 255),
            -1 
        );

        string label = cv::format(
            "%s X:%.1f Y:%.1f Z:%.1f mm",
            namaWarna.c_str(), X, Y, Z
        );

        cv::putText(
            frame_result,
            label,
            cv::Point(
                boundingBox.x,
                max(20, boundingBox.y - 10)
            ),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            warnaKotak,
            2
        );

        cout << label << endl;
    }
}

int main() {
    cv::VideoCapture cap(2);

    if (!cap.isOpened()) {
        cerr << "Error: Kamera gagal dibuka!" << endl;
        return -1;
    }

    // Membaca matriks kalibrasi satu kali
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

    cv::Mat frame, hsl_video;
    cv::Mat blueMask, redMask;
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
            cv::Scalar(85, 50, 50),
            cv::Scalar(130, 255, 255),
            blueMask
        );

        cv::inRange(
            hsl_video,
            cv::Scalar(0, 50, 50),
            cv::Scalar(10, 255, 255),
            redMask1
        );

        cv::inRange(
            hsl_video,
            cv::Scalar(170, 50, 50),
            cv::Scalar(180, 255, 255),
            redMask2
        );

        cv::bitwise_or(
            redMask1,
            redMask2,
            redMask
        );
        frame_result = frame.clone();

        deteksiWarna(
            blueMask, frame_result, "BIRU",
            cv::Scalar(255, 0, 0),
            fx, fy, cx, cy, 700.0
        );

        deteksiWarna(
            redMask, frame_result, "MERAH",
            cv::Scalar(0, 0, 255),
            fx, fy, cx, cy, 700.0
        );

        cv::imshow("Deteksi Dua Warna", frame_result);
        cv::imshow("Mask Biru", blueMask);
        cv::imshow("Mask Merah", redMask);

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
