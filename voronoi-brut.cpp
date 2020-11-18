#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>
#include <argparse/argparse.hpp>
#include <chrono>

using namespace cv;
using namespace std;
using namespace chrono;

double de (const Point & a, const Point & b) {
    double distanceX = a.x - b.x;
    double distanceY = a.y - b.y;

    double distanceSquared = pow(distanceX, 2) + pow(distanceY, 2);

    return sqrt(distanceSquared);

}

double d1 (const Point & a, const Point & b) {
    double distanceX = abs(a.x - b.x);
    double distanceY = abs(a.y - b.y);

    return distanceX + distanceY;
}

double dI (const Point & a, const Point & b) {
    double distanceX = abs(a.x - b.x);
    double distanceY = abs(a.y - b.y);

    return distanceX > distanceX
        ? distanceX
        : distanceY;
}

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

tuple<time_point<steady_clock>, int> run (int rows, int columns, int pointCount) {

    int iterationCount = 0;
    Mat i = Mat(rows, columns, CV_8UC3);
    auto randomPoints = createRandomPoints(i, pointCount);

    for (int x = 0; x < rows; ++x) {
        for (int y = 0; y < columns; ++y) {
            Point p(x, y);
            double lowestDistance = numeric_limits<double>::max();
            auto closestPoint = tuple<Vec3b, Point> (Vec3b(0, 0, 0), Point( -1, -1));

            for (auto & point : randomPoints)
            {
                const auto currentDistance = de(p, get<1>(point));
                ++iterationCount;

                if (currentDistance < lowestDistance) {
                    lowestDistance = currentDistance;
                    closestPoint = point;
                }
            }

            i.at<Vec3b>(p) = get<0>(closestPoint);
        }
    }

    auto end = high_resolution_clock::now();

    namedWindow("result", WINDOW_AUTOSIZE);
    imshow("result", i);
    waitKey(0);

    return tuple<time_point<steady_clock>, int>(end, iterationCount);
}

string getArgumentName (string name) {
    return "--" + name;
}

int main(int argc, const char ** argv) {

    ArgumentParser argumentParser;

    const string columnCountParameterName = "columnCount";
    const string rowCountParameterName = "rowCount";
    const string pointCountParameterName = "germs";

    argumentParser.addArgument(getArgumentName(columnCountParameterName), 1, false);
    argumentParser.addArgument(getArgumentName(rowCountParameterName), 1, false);
    argumentParser.addArgument(getArgumentName(pointCountParameterName), 1, false);

    argumentParser.parse(argc, argv);

    auto columnCountString = argumentParser.retrieve<string>(columnCountParameterName);
    auto rowCountString = argumentParser.retrieve<string>(rowCountParameterName);
    auto pointCountString = argumentParser.retrieve<string>(pointCountParameterName);

    auto columnCount = stoi(columnCountString);
    auto rowCount = stoi(rowCountString);
    auto pointCount = stoi(pointCountString);

    auto start = high_resolution_clock::now();
    auto result = run(rowCount, columnCount, pointCount);

    auto duration = duration_cast<milliseconds>(get<0>(result) - start);

    cout
        << "Execution done in " << duration.count() << " ms and " << to_string(get<1>(result)) << " iterations."
        << " Create a " << rowCountString << " x " << columnCountString << " (" << to_string(columnCount * rowCount) <<  " pixels) image with " << pointCount << " germs." << endl;

    return 0;
}
