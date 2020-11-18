#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

vector<tuple<Vec3b, Point>> createRandomPoints (Mat & m, int pointCount = 100) {
    vector<tuple<Vec3b, Point>> randomPoints;

    auto rows = m.rows;
    auto columns = m.cols;

    for (auto i = 0; i < pointCount; ++i) {
        auto randomX = random() % (rows - 1);
        auto randomY = random() % (columns - 1);

        auto randomRed = random() % 255;
        auto randomGreen = random() % 255;
        auto randomBlue = random() % 255;

        randomPoints.emplace_back(Vec3b(randomRed, randomGreen, randomBlue), Point(randomX, randomY));
    }

    return randomPoints;
}