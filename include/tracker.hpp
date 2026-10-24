#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Tracker { 
    private:
        double fx, fy, cx, cy;

    public:
        Tracker(double fx, double fy, double cx, double cy){
            this->fx = fx;
            this->fy = fy;
            this->cx = cx;
            this->cy = cy;
        }  
        
        void deteksiWarna(
        const cv::Mat& mask,
        cv::Mat& frame_result,
        const string& namaWarna,
        cv::Scalar warnaKotak,
        double lebarAsli) {

        cv::Mat morphImage;

        cv::erode(
            mask, morphImage, cv::Mat(),
            cv::Point(-1, -1), 1
        );

        cv::dilate(
            morphImage, morphImage, cv::Mat(),
            cv::Point(-1, -1), 5
        );

        // cv::erode(
        // mask, morphImage, cv::Mat(),
        // cv::Point(-1, -1), 1
        // );


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

            double u = m.m10 / m.m00;
            double v = m.m01 / m.m00;

            double Z = (fx * lebarAsli) / wpixel;
            double X = ((u - cx) * Z) / fx;
            double Y = ((v - cy) * Z) / fy;

            cv::rectangle(frame_result, boundingBox, warnaKotak, 2);

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

            cv::putText(frame_result, label, cv::Point(boundingBox.x,max(20, boundingBox.y - 10)), cv::FONT_HERSHEY_SIMPLEX, 0.5, warnaKotak, 2);
        }
    }
};

#endif