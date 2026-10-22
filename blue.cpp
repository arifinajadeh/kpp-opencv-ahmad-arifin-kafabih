
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    cv::VideoCapture cap(2);

    if (!cap.isOpened()) {
        std::cerr << "Gagal membuka kamera!" << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::Mat resized_frame;

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Gagal membaca video!" << std::endl;
            break;
        }

        cv::resize(frame, resized_frame, cv::Size(640, 480));

        cv::Mat hls_image;
        cv::cvtColor(resized_frame, hls_image, cv::COLOR_BGR2HLS);

        cv::Mat thresholdedImage;
        cv::inRange(
            hls_image,
            cv::Scalar(85, 25, 50),
            cv::Scalar(125, 200, 255),
            thresholdedImage
        );

        cv::Mat morphology = thresholdedImage.clone();

        cv::erode(
            morphology, morphology, cv::Mat(),
            cv::Point(-1, -1), 5
        );

        cv::dilate(
            morphology, morphology, cv::Mat(),
            cv::Point(-1, -1), 3
        );

        cv::erode(
            morphology, morphology, cv::Mat(),
            cv::Point(-1, -1), 1
        );

        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(
            morphology,
            contours,
            cv::RETR_EXTERNAL,
            cv::CHAIN_APPROX_SIMPLE
        );

        cv::Mat result = resized_frame.clone();

        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);

            if (area > 3000) {
                cv::Rect boundingBox =
                    cv::boundingRect(contours[i]);

                cv::rectangle(
                    result,
                    boundingBox,
                    cv::Scalar(0, 255, 0),
                    2
                );
            }
        }

        cv::imshow("Gambar Asli", resized_frame);
        cv::imshow("Gambar HLS", hls_image);
        cv::imshow("Threshold", thresholdedImage);
        cv::imshow("Morphology", morphology);
        cv::imshow("Bounding Box", result);

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
