#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <create-random-points.hpp>
#include <is-in-mat.hpp>

using namespace std;
using namespace chrono;
using namespace cv;

vector<tuple<Vec3b, Point>> createClasses (Mat & mat, int classCount) {
    return createRandomPoints(mat, classCount);
}

vector<tuple<Point, int>> createMask () {
    vector<tuple<Point, int>> mask;

    for (int y = -1; y < 2; ++y) {
        for (int x = -1; x < 2; ++x) {
            mask.emplace_back(Point(x, y), x == 0 && y == 0 ? 0 : 1);
        }
    }

    return mask;
}

int main (int argc, char ** argv) {
    const int columnCount = 100;
    const int rowCount = 100;

    const int kCount = 100;
    const int maxIterationCount = 4;

    Mat voronoi(rowCount, columnCount, CV_8UC3);
    Mat distances(rowCount, columnCount, CV_16UC1);

    auto classes = createClasses(voronoi, kCount);
    const auto mask = createMask();

    distances = Scalar::all(numeric_limits<ushort>::infinity());

    // Initialise voronoi matrix with black pixels
    for (int i = 0; i < voronoi.rows; ++i) {
        for (int j = 0; j < voronoi.cols; ++j) {
            voronoi.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
        }
    }

    for (auto _class : classes) {
        voronoi.at<Vec3b> (get<1>(_class)) = get<0>(_class);
    }

    int iterationCount = 0;
    int oldClassesCount = 0;

    do {
        cout << to_string(iterationCount) << endl;
        oldClassesCount = classes.size();

        const auto classesCopy = classes;

        for (auto _class : classesCopy) {
            auto color = get<0>(_class);
            auto point = get<1>(_class);
            auto middlePointWeight = distances.at<ushort>(point);

            for (auto maskElement : mask) {
                auto relativePoint = get<0> (maskElement);
                auto maskWeight = get<1> (maskElement);

                auto absolutePoint = Point(relativePoint.x + point.x, relativePoint.y + point.y);

                if (!isInMat(voronoi, absolutePoint))
                    continue;

                auto currentWeight = distances.at<ushort> (absolutePoint);
                auto currentWeightInteger = (int)currentWeight;

                if (currentWeight == numeric_limits<ushort>::infinity() || middlePointWeight + maskWeight <= currentWeightInteger) {
                    distances.at<uchar>(absolutePoint) = middlePointWeight + maskWeight;
                    voronoi.at<Vec3b>(absolutePoint) = color;
                    classes.emplace_back(color, absolutePoint);
                }
            }
        }

        ++iterationCount;
    } while (iterationCount < maxIterationCount && oldClassesCount != classes.size());

    namedWindow("voronoi", WINDOW_AUTOSIZE);
    imshow("voronoi", voronoi);

    waitKey(0);

    return 0;
}