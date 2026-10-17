
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main (){
    cv::VideoCapture cap(0);
    cv::Mat frame;
    cv::Mat resized_frame;  
    while (cap.isOpened()) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Error: Could not read frame from camera." << std::endl;
            break;
        }   
        cv::resize(frame, resized_frame, cv::Size(640, 480));
    if (frame.empty()) {
        std::cerr << "Gagal membaca video!" << std::endl;
        return -1;
    }

    cv::Mat hls_image;
    cv::cvtColor(resized_frame, hls_image, cv::COLOR_BGR2HLS);

    // 2. Membuat threshold berdasarkan rentang HLS
    cv::Mat thresholdedImage;
    cv::inRange(
        hls_image,
        cv::Scalar(85, 50, 100),
        cv::Scalar(125, 200, 255),
        thresholdedImage
    );

    // 3. Morphology untuk membersihkan mask
    cv::Mat morphology = thresholdedImage.clone();

    cv::erode(morphology, morphology, cv::Mat(), cv::Point(-1, -1), 2);
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(
        morphology,
        contours,
        cv::RETR_EXTERNAL,
        cv::CHAIN_APPROX_SIMPLE
    );

    // 5. Menyiapkan gambar hasil
    cv::Mat result = resized_frame.clone();

    // 6. Membuat bounding box
    for (size_t i = 0; i < contours.size(); i++) {

        double area = cv::contourArea(contours[i]);

        if (area > 1000) {
            cv::Rect boundingBox =
                cv::boundingRect(contours[i]);

            cv::rectangle(
                result,
                boundingBox,
                cv::Scalar(0, 255, 0),
                2
            );

            std::cout << "Objek " << i + 1
                      << " | Luas: " << area
                      << " | X: " << boundingBox.x
                      << " | Y: " << boundingBox.y
                      << " | Lebar: " << boundingBox.width
                      << " | Tinggi: " << boundingBox.height
                      << std::endl;
        }
    }

    // 7. Menampilkan hasil
    cv::imshow("Gambar Asli", resized_frame);
    cv::imshow("Gambar HLS", hls_image);
    cv::imshow("Threshold", thresholdedImage);
    cv::imshow("Morphology", morphology);
    cv::imshow("Bounding Box", result);

    cv::waitKey(0);

    return 0;
