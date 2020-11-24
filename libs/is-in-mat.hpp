#include <opencv2/opencv.hpp>

using namespace cv;

bool isInMat (const Mat & mat, const Point & point) {
    return point.x >= 0 && point.y >= 0 && point.x < mat.rows && point.y < mat.cols;
}