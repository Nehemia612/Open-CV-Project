#include <opencv2/opencv.hpp>
#include <iostream>

int main(){
    //cv::VideoCapture cap("/dev/v4l/by-id/usb-046d_C270_HD_WEBCAM_98CF6D60-video-index0");
    cv::VideoCapture cap("http://10.193.146.193:8080/video");
    cv::Mat frame;
    
    if(!cap.isOpened()){
            std::cout << "Error gabisa buka kamera"<<std::endl;
            return -1;
    }
    cv::Mat camera_matrix;
    cv::Mat distortion_coefficients;
    cv::Mat resized, hlsFrame;
    std::vector<cv::Mat> rvecs, tvecs;
        
    cv::FileStorage fs("kalibrasi.yaml", cv::FileStorage::READ);
    if(fs.isOpened()){
        fs["camera_matrix"] >> camera_matrix;
        fs["distortion_coefficients"] >> distortion_coefficients;

        fs.release();
        std::cout << "[SUKSES] BERHASIL MEMBUKA FILE YAML!" << std::endl;
        std::cout <<"Camera Matrix: " << camera_matrix << std::endl;
        std::cout <<"Distortion Coeffisien: " << distortion_coefficients << std::endl;
    } else {
        std::cout << "[ERROR] Gagal membuka file YAML!" << std::endl;
    }

    double fx = camera_matrix.at<double>(0,0);
    double fy = camera_matrix.at<double>(1,1);
    double cx = camera_matrix.at<double>(0,2);
    double cy = camera_matrix.at<double>(1,2);
    double x,y,z;


    while(cap.isOpened()){
        double fps = 0.0;
        int64 waktu_mulai = cv::getTickCount();

        cap >> frame;
        if(frame.empty()){
            std::cout << "Framenya KOSONG!" << std::endl;
            return -1;
        }
        cv::resize(frame, resized, cv::Size(640, 480));
        cv::cvtColor(resized, hlsFrame, cv::COLOR_BGR2HLS);

        cv::Mat gabungan;
        cv::cvtColor(resized, hlsFrame, cv::COLOR_BGR2HLS);
        cv::Mat merah, biru, hijau;
        cv::inRange(hlsFrame, cv::Scalar (93, 70, 45), cv::Scalar (180, 255, 255), biru);
        cv::inRange(hlsFrame, cv::Scalar (0, 0, 43), cv::Scalar (18, 255, 255), merah);
        cv::inRange(hlsFrame, cv::Scalar (61, 0, 23), cv::Scalar (90, 217, 217), hijau);
        cv::bitwise_or(biru, merah, gabungan);
        cv::bitwise_or(gabungan, hijau, gabungan);

        std::vector<cv::Mat> color = {biru, merah, hijau};
        std::vector<std::string> label = {"sticky note biru", "tutup botol merah", "sponge cuci piring"};
        std::vector<double> w_real = {7.6, 9.0, 10.8};

        for(int j = 0; j < 3; j++){
            cv::Mat morph = color[j].clone();
            cv::erode(morph, morph, cv::Mat(), cv::Point(-1, -1), 2);
            cv::dilate(morph, morph, cv::Mat(), cv::Point(-1, -1), 2);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(morph, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            for (size_t i = 0; i < contours.size(); i++) {
                if (cv::contourArea(contours[i]) > 2000) {
                    cv::Rect boundingBox = cv::boundingRect(contours[i]);
                    cv::rectangle(resized, boundingBox, cv::Scalar(0, 255, 0), 2);
                    int x_pixel = boundingBox.x + boundingBox.width / 2;
                    int y_pixel = boundingBox.y + boundingBox.height / 2;
                    cv::circle(resized, cv::Point(x_pixel, y_pixel), 5, cv::Scalar(0, 0, 255), -1);
                    double w_pixel = boundingBox.width;

                    z = (fx * w_real[j]) / w_pixel;
                    x = ((x_pixel - cx)*z) / fx;
                    y = ((y_pixel - cy)*z) / fy;

                    std::string koordinat = cv::format("(%.2lf, %.2lf, %.2lf)", x, y, z);
                    std::string jarak_z = cv::format("Jarak Z = %.2lf cm", z);
                    std::string nama_objek = "Nama Objek: " + (label[j]);
                    std::string lebar_objek = "Lebar Objek: " + std::to_string((double)w_real[j]);

                    std::cout << "Objek " << label[j] << " Koordinatnya di X: " << x << ", Y: " << y << ", Z: "<< z << "dengan jarak: " << z << "cm" << std::endl;
                    cv::putText(resized, koordinat, cv::Point (20,60), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
                    cv::putText(resized, jarak_z, cv::Point (20,80), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 0), 2);
                    cv::putText(resized, lebar_objek, cv::Point (20,100), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
                    cv::putText(resized, nama_objek, cv::Point (20,120), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 0), 2);


                }
            }

            int64 waktu_selesai = cv::getTickCount();
            double waktu_per_frame = (waktu_selesai - waktu_mulai) / cv::getTickFrequency();
            fps = 1.0 / waktu_per_frame;

            std::string teksFps = "FPS: " + std::to_string((int)fps);
            cv::putText(resized, teksFps, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
            
        }
        cv::imshow("Kamera Asli", resized);

        char key = (char) cv::waitKey(30);
        if(key == 'q'){
            break;
        }
    }

    




    return 0;
}
