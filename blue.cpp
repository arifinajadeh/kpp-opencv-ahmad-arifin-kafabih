#include <opencv2/opencv.hpp>
#include <iostream>

int main (){
    cv::Mat image = cv::imread("../image_n_video/photos/sample2.jpg");
    
    cv:: Mat hls_image;
    cv::cvtColor(image, hls_image, cv::COLOR_BGR2HLS);


    cv:: Mat thresholdedImage;

    cv::inRange(image, 
        cv::Scalar(11, 42, 61), 
        cv::Scalar(255, 211, 255), 
        thresholdedImage);

    cv:: Mat morphology  = thresholdedImage.clone();

    cv::erode(morphology, morphology, cv::Mat(), cv::Point(-1,-1), 2);
    cv::dilate(morphology, morphology, cv::Mat(), cv::Point(-1,-1), 8);
    cv::erode(morphology, morphology, cv::Mat(), cv::Point(-1,-1), 5);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(morphology, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat result = image.clone()

    for (size_t i = 0; i < contours.size() ; i++){
        if(cv::contourArea(contours[i] > 1000)){
            cv::Rect boundingBox = 
        }
    }
    cv::imshow("Gambar", image);
    cv::imshow("Gambar HLS", hls_image);    
    cv::waitKey(0);
    return 0;
}