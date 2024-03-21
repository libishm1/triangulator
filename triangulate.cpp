#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    FileStorage fs("camera.xml", FileStorage::READ);

    // Left Camera Data
    Mat leftCameraMatrix, leftDistCoeffs, leftRvec, leftTvec;
    vector<vector<Point2f>> leftImagePoints;

    fs["left_camera"]["camera_matrix"] >> leftCameraMatrix;
    fs["left_camera"]["dist_coeffs"] >> leftDistCoeffs;
    fs["left_camera"]["rvec"] >> leftRvec;
    fs["left_camera"]["tvec"] >> leftTvec;
    fs["left_camera"]["image_points"] >> leftImagePoints;

    // Right Camera Data
    Mat rightCameraMatrix, rightDistCoeffs, rightRvec, rightTvec;
    vector<vector<Point2f>> rightImagePoints;

    fs["right_camera"]["camera_matrix"] >> rightCameraMatrix;
    fs["right_camera"]["dist_coeffs"] >> rightDistCoeffs;
    fs["right_camera"]["rvec"] >> rightRvec;
    fs["right_camera"]["tvec"] >> rightTvec;
    fs["right_camera"]["image_points"] >> rightImagePoints;

    fs.release();

    // Scale camera matrices and distortion coefficients for resolution
    Size leftResolution(1920, 1280);
    Size rightResolution(6000, 4000);
    double scaleLeft = static_cast<double>(leftResolution.width) / leftCameraMatrix.cols;
    double scaleRight = static_cast<double>(rightResolution.width) / rightCameraMatrix.cols;

    leftCameraMatrix *= scaleLeft;
    leftDistCoeffs *= scaleLeft;
    rightCameraMatrix *= scaleRight;
    rightDistCoeffs *= scaleRight;

    // Construct projection matrices
    Mat leftProjectionMatrix = Mat::eye(3, 4, CV_64F);
    Mat rightProjectionMatrix = Mat::eye(3, 4, CV_64F);
    leftCameraMatrix.copyTo(leftProjectionMatrix(Rect(0, 0, 3, 3)));
    rightCameraMatrix.copyTo(rightProjectionMatrix(Rect(0, 0, 3, 3)));

    // Extract first fifteen image points for both left and right cameras
    vector<Point2f> leftFirstFifteenPoints, rightFirstFifteenPoints;
    for (int i = 0; i < 15; ++i) {
        leftFirstFifteenPoints.push_back(leftImagePoints[i][0]);
        rightFirstFifteenPoints.push_back(rightImagePoints[i][0]);
    }

    // Convert to Mat format for triangulation
    Mat leftFirstFifteenPointsMat(2, 15, CV_32FC1);
    Mat rightFirstFifteenPointsMat(2, 15, CV_32FC1);
    for (int i = 0; i < 15; ++i) {
        leftFirstFifteenPointsMat.at<float>(0, i) = leftFirstFifteenPoints[i].x;
        leftFirstFifteenPointsMat.at<float>(1, i) = leftFirstFifteenPoints[i].y;
        rightFirstFifteenPointsMat.at<float>(0, i) = rightFirstFifteenPoints[i].x;
        rightFirstFifteenPointsMat.at<float>(1, i) = rightFirstFifteenPoints[i].y;
    }

    // Perform triangulation
    Mat points4D;
    triangulatePoints(leftProjectionMatrix, rightProjectionMatrix,
                      leftFirstFifteenPointsMat, rightFirstFifteenPointsMat, points4D);

    // Print the scaled camera matrices and distortion coefficients
    cout << "Scaled Left Camera Matrix:\n" << leftCameraMatrix << endl;
    cout << "Scaled Left Distortion Coefficients:\n" << leftDistCoeffs << endl;
    cout << "Scaled Right Camera Matrix:\n" << rightCameraMatrix << endl;
    cout << "Scaled Right Distortion Coefficients:\n" << rightDistCoeffs << endl;

    // Print the triangulated points
    cout << "Triangulated Points:\n" << points4D << endl;

    return 0;
}

//The code reads camera calibration data from an XML file and extracts left and right camera matrices, distortion coefficients, rotation vectors, translation vectors, and image points.
//It scales the camera matrices and distortion coefficients based on the resolution of the cameras.
//It constructs projection matrices using the scaled camera matrices.
//It extracts the first fifteen image points for both left and right cameras.
//It converts the image points to Mat format for triangulation.
//It performs triangulation using OpenCV's triangulatePoints function and prints the scaled camera matrices, distortion coefficients, and triangulated points.

