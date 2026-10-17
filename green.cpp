#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
int main (){
    cv::VideoCapture cap(0);
    cv::Mat frame;
    cv::Mat resized_frame;
    cv::Mat hsl_video;
    cv::Mat thresholdedImage;
    cv::Mat morphImage;
    while (cap.isOpened()) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Error: Could not read frame from camera." << std::endl;
            break;
        }   

        cv::cvtColor(frame, hsl_video, cv::COLOR_BGR2HLS);
        cv::Mat blue, red;

        cv::inRange(hsl_video,
        cv::Scalar (85, 50, 50),
        cv::Scalar (130, 255, 255),
        thresholdedImage);




        cv::erode(thresholdedImage, morphImage, cv::Mat(), cv::Point(-1, -1), 8);
        cv::dilate(morphImage, morphImage, cv::Mat(), cv::Point(-1, -1), 5);
        cv::erode(morphImage, morphImage, cv::Mat(), cv::Point(-1, -1), 10);

        vector<vector<cv::Point>> contours;
        cv::findContours(morphImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        cv::Mat frame_result = frame.clone();
        for (size_t i = 0; i < contours.size(); i++){
            if(cv::contourArea(contours[i]) > 1000){
                cv::Rect boundingBox = cv::boundingRect(contours[i]);
                cv::rectangle(frame_result, boundingBox, cv::Scalar(0, 255, 0), 2);
                double wpixel = boundingBox.width;

                cv::Mat K;
                cv::FileStorage fs ("../yaml/kalibrasi.yaml", cv::FileStorage::READ);
                fs["K"] >> K;
                fs.release();

                cv::Moments m = cv::moments(morphImage, true);
                if (m.m00 == 0 || wpixel == 0) {
                        continue;
                    }
                double u = m.m10 / m.m00;
                double v = m.m01 / m.m00;



                double fx = K.at<double>(0, 0);
                double fy = K.at<double>(1, 1);
                double cx = K.at<double>(0, 2);
                double cy = K.at<double>(1, 2);

                double Z = (fx * 700.0) / wpixel;
                double X = ((u - cx) * Z) / fx;
                double Y = ((v - cy) * Z) / fy;

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

                string koordinat = cv::format(
                    "X: %.1f Y: %.1f Z: %.1f mm",
                    X, Y, Z
                );

                cv::putText(
                    frame_result,
                    koordinat,
                    cv::Point(
                        boundingBox.x,
                        max(20, boundingBox.y - 10)
                    ),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(0, 255, 0),
                    2
                );
                cout << koordinat << endl;
            }
        }
        cv::resize(
            frame_result,
            resized_frame,
            cv::Size(400, 480)
        );

        cv::imshow("Video Resize", resized_frame);
        cv::imshow("Threshold", thresholdedImage);
        cv::imshow("Morphology", morphImage);
        cv::imshow("main", frame_result);

        if (cv::waitKey(1) == 'q') {
            break;
            }
        }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}