#include <iostream>
#include <opencv2/opencv.hpp>
#include <create-random-points.hpp>
#include <chrono>
#include <argparse/argparse.hpp>
#include <get-argument-name.hpp>

using namespace std;
using namespace chrono;
using namespace cv;

vector<tuple<Point, int>> getFrontMask () {
    vector<tuple<Point, int>> points;

    points.emplace_back(Point(-1, -1), 1);
    points.emplace_back(Point(-1, 0), 1);
    points.emplace_back(Point(1, -1), 1);
    points.emplace_back(Point(0, -1), 1);
    points.emplace_back(Point(0, 0), 0);

    return points;
}

vector<tuple<Point, int>> getBackMask () {
    vector<tuple<Point, int>> points;

    points.emplace_back(Point(0, 1), 1);
    points.emplace_back(Point(1, 1), 1);
    points.emplace_back(Point(1, 0), 1);
    points.emplace_back(Point(-1, 1), 1);
    points.emplace_back(Point(0, 0), 0);

    return points;
}

const int FAKE_INFINITY = 257;

short add (short a, short b) {
    if (a == FAKE_INFINITY || b == FAKE_INFINITY) return FAKE_INFINITY;

    return a + b;
}

bool isInMat (const Mat & mat, const Point & point) {
    return point.x >= 0 && point.y >= 0 && point.x < mat.rows && point.y < mat.cols;
}

void printMat (const Mat & mat) {
        for (int j = 0; j < mat.rows; ++j) {
            for (int i = 0; i < mat.cols; ++i) {
            auto color = mat.at<Vec3b>(j, i);
            cout << " (" <<  to_string(color[0]) << ", " << to_string(color[1]) << ", " << to_string(color[2]) << ")";
        }

        cout << endl;
    }
}

void printMatShort (const Mat & mat) {
    for (int j = 0; j < mat.rows; ++j) {
        for (int i = 0; i < mat.cols; ++i) {
            auto color = mat.at<short>(j, i);
            cout << " (" <<  to_string(color) << ")";
        }

        cout << endl;
    }
}

tuple<time_point<steady_clock>, int> run (int rowCount, int columnCount, int germsCount) {
    int iterationCount = 0;

    Mat distances (rowCount, columnCount, CV_16SC1);
    Mat voronoi (rowCount, columnCount, CV_8UC3);
//    vector<tuple<Vec3b, Point>> randomPoints;
//    randomPoints.emplace_back(Vec3b(111, 111, 111), Point(1, 2));
//    randomPoints.emplace_back(Vec3b(222, 222, 222), Point(3, 3));
//    randomPoints.emplace_back(Vec3b(0, 0, 0), Point(4, 5));
    auto randomPoints = createRandomPoints(distances, germsCount);

    distances = Scalar::all(FAKE_INFINITY);

    for (int i = 0; i < voronoi.rows; ++i) {
        for (int j = 0; j < voronoi.cols; ++j) {
            voronoi.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
        }
    }

    for (auto colorAndPoint : randomPoints) {
        auto point = get<1> (colorAndPoint);
        distances.at<short>(point) = 0;
        voronoi.at<Vec3b> (point) = get<0>(colorAndPoint);
    }

    auto frontMask = getFrontMask();

//
    for (int y = 0; y < rowCount; ++y) {
        for (int x = 0; x < columnCount; ++x) {

            short minFound = FAKE_INFINITY;
            Point minFoundPoint = Point(-1,-1);

            for (auto element : frontMask) {
                auto localPoint = get<0>(element);
                auto absolutePoint = Point(localPoint.x + x, localPoint.y + y);

                if (!isInMat(distances, absolutePoint)) continue;

                auto weight = distances.at<short> (absolutePoint);
                auto currentWeight = add(weight, get<1>(element));

                ++iterationCount;

                if (currentWeight <= minFound) {
                    minFound = currentWeight;
                    minFoundPoint = absolutePoint;
                }
            }

            voronoi.at<Vec3b> (y ,x) = voronoi.at<Vec3b> (minFoundPoint);
            distances.at<short> (y, x) = minFound;
        }
    }

    auto backMask = getBackMask();

    for (int y = rowCount - 1; y >= 0; --y) {
        for (int x = columnCount - 1; x >= 0; --x) {

            short minFound = FAKE_INFINITY;
            Point minFoundPoint = Point(-1, -1);

            for (auto element : backMask) {
                auto localPoint = get<0>(element);
                auto absolutePoint = Point(localPoint.x + x, localPoint.y + y);

                if (!isInMat(distances, absolutePoint)) continue;

                auto weight = distances.at<short> (absolutePoint);
                auto currentWeight = add(weight, get<1>(element));

                ++iterationCount;

                if (currentWeight <= minFound) {
                    minFound = currentWeight;
                    minFoundPoint = absolutePoint;
                }
            }
            voronoi.at<Vec3b> (y ,x) = voronoi.at<Vec3b> (minFoundPoint);
            distances.at<short> (y, x) = minFound;
        }
    }

    auto end = high_resolution_clock::now();

    namedWindow("result", WINDOW_AUTOSIZE);
    imshow("result", voronoi);
    waitKey(0);

    return tuple<time_point<steady_clock>, int> (end, iterationCount);
}

int main (int argc, const char ** argv) {

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

    cout << "Execution done in " << duration.count() << " ms and " << to_string(get<1>(result)) << " iteration. " << endl
        << "Create a " << rowCountString << " x " << columnCountString << " ("<< to_string(columnCount * rowCount) << " pixels) image with " << pointCountString << " germs. " << endl
        << to_string(((double)pointCount / (double)(columnCount * rowCount)) * 100.0) << "% of the points in this image are germs.";

    return 0;
}