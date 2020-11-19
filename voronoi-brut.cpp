#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>
#include <argparse/argparse.hpp>
#include <create-random-points.hpp>
#include <chrono>
#include <get-argument-name.hpp>

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

string distanceNames[] = {
    "DE",
    "D1",
    "DInfinity"
};

const int DISTANCE_DE = 0;
const int DISTANCE_D1 = 1;
const int DISTANCE_DI = 2;

double getDistance (int distanceType, const Point & a, const Point & b) {
    switch(distanceType) {
        case DISTANCE_D1:
            return d1(a, b);
        case DISTANCE_DE:
            return de(a, b);
        case DISTANCE_DI:
            return d1(a, b);
    }

    return -1;
}


tuple<time_point<steady_clock>, int> run (int rows, int columns, int pointCount, int distanceType, bool skipImage = false) {

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
                const auto currentDistance = getDistance(distanceType, p, get<1>(point));
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

    if (!skipImage) {

        namedWindow("result", WINDOW_AUTOSIZE);
        imshow("result", i);
        waitKey(0);
    }

    return tuple<time_point<steady_clock>, int>(end, iterationCount);
}


int main(int argc, const char ** argv) {

    ArgumentParser argumentParser;

    const string columnCountParameterName = "columnCount";
    const string rowCountParameterName = "rowCount";
    const string pointCountParameterName = "germs";
    const string distanceTypeParameterName = "distanceType";
    const string skipImageString = "skipImage";

    argumentParser.addArgument(getArgumentName(columnCountParameterName), 1, false);
    argumentParser.addArgument(getArgumentName(rowCountParameterName), 1, false);
    argumentParser.addArgument(getArgumentName(pointCountParameterName), 1, false);
    argumentParser.addArgument(getArgumentName(distanceTypeParameterName), 1, false);
    argumentParser.addArgument(getArgumentName(skipImageString), 0, true);

    argumentParser.parse(argc, argv);

    auto columnCountString = argumentParser.retrieve<string>(columnCountParameterName);
    auto rowCountString = argumentParser.retrieve<string>(rowCountParameterName);
    auto pointCountString = argumentParser.retrieve<string>(pointCountParameterName);
    auto distanceTypeString = argumentParser.retrieve<string>(distanceTypeParameterName);
    auto skipImage = argumentParser.exists(skipImageString);

    auto columnCount = stoi(columnCountString);
    auto rowCount = stoi(rowCountString);
    auto pointCount = (int)round(stof(pointCountString));
    auto distanceType = stoi(distanceTypeString);

    auto start = high_resolution_clock::now();
    auto result = run(rowCount, columnCount, pointCount, distanceType, skipImage);

    auto duration = duration_cast<milliseconds>(get<0>(result) - start);

    cout
        << "Execution done in " << duration.count() << " ms and " << to_string(get<1>(result)) << " iterations."
        << " Create a " << rowCountString << " x " << columnCountString << " (" << to_string(columnCount * rowCount) <<  " pixels) image with " << pointCount << " germs and distance " << distanceNames[distanceType] << "." << endl;

    return 0;
}
