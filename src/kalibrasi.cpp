#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>

int main()
{
    cv::Size boardSize(8, 6);
    cv::VideoCapture cap("http://10.12.251.218:8080/video");
    // cv::VideoCapture cap("/dev/v4l/by-id/usb-046d_C270_HD_WEBCAM_98CF6D60-video-index0");
    cv::Mat frame;
    cv::Mat resized;
    cv::Mat grayscale;
    float squareSize = 23.0f;

    std::vector<std::vector<cv::Point3f>> objectpoints;
    std::vector<std::vector<cv::Point2f>> imagepoints;

    std::vector<cv::Point3f> objp;
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            objp.push_back(cv::Point3f(j * squareSize, i * squareSize, 0.0f));
        }
    }

    std::cout << "=== INSTRUKSI ===" << std::endl;
    std::cout << "Arahkan kamera ke chessboard, lalu tekan [SPASI] untuk ambil snapshot." << std::endl;
    std::cout << "Tekan [Q] untuk keluar." << std::endl;

    while(cap.isOpened()){
        cap >> frame;
        cv::resize(frame, resized, cv::Size(640, 480));

        cv::cvtColor(resized, grayscale, cv::COLOR_BGR2GRAY);
        if(grayscale.empty()){
            break;
        }
        
        std::vector<cv::Point2f> corners;
        bool result = cv::findChessboardCorners(grayscale, boardSize, corners);

        if(result){
            cv::drawChessboardCorners(resized, boardSize, corners, true);
        }
        char key = cv::waitKey(1);
        cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);
        if(key == ' '){
            if(result){
                cv::cornerSubPix(
                    grayscale,
                    corners,
                    cv::Size(11, 11),
                    cv::Size(-1, -1),
                    criteria

                );
                objectpoints.push_back(objp);
                imagepoints.push_back(corners);
                std::cout << "Snapshot berhasil diambil! Total terkumpul: " << objectpoints.size() << std::endl;
            }else{
                std::cout << "Snapshot tidak berhasil diambil! "<< std::endl;
            }

            
        } 
        if(key == 'c' || key == 'C'){
            if(objectpoints.size() < 15){
                std::cout<<"Data snapshot yang dibutuhkan masih kurang! Saat ini: " << objectpoints.size() << std::endl;
            }else{
                cv::Mat camera_matrix;
                cv::Mat distortion_coefficients;
                std::vector<cv::Mat> rvecs, tvecs;

                double rms = cv::calibrateCamera(objectpoints, imagepoints, resized.size(), camera_matrix, distortion_coefficients, rvecs, tvecs);
                std::cout << "========================================" << std::endl;
                std::cout << "KALIBRASI SELESAI!" << std::endl;
                std::cout << "RMS Reprojection Error: " << rms << std::endl;
                std::cout << "Camera Matrix (K):\n" << camera_matrix << std::endl;
                std::cout << "========================================" << std::endl;

                cv::FileStorage fs("kalibrasi.yaml", cv::FileStorage::WRITE);
                if(fs.isOpened()){
                    fs << "camera_matrix" << camera_matrix;
                    fs << "distortion_coefficients" << distortion_coefficients;
                    fs << "focal_length_x" << camera_matrix.at<double>(0, 0);
                    fs << "focal_length_y" << camera_matrix.at<double>(1, 1);
                    fs << "principal_point_x" << camera_matrix.at<double>(0, 2);
                    fs << "principal_point_y" << camera_matrix.at<double>(1, 2);
                    fs << "board_width" << boardSize.width;
                    fs << "board_height" << boardSize.height;
                    fs << "square_size" << squareSize;
                    fs.release();
                    std::cout << "[SUKSES] Parameter berhasil disimpan ke 'kalibrasi.yaml'!" << std::endl;
                } else {
                    std::cout << "[ERROR] Gagal membuka file untuk menyimpan YAML!" << std::endl;
                }
            }
        }

        cv::imshow("Kamera", resized);
        if(key == 'q'){
            break;
        }
    }



  return 0;
}