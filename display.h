#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class Display
{ 
public:

    void displaytrajectory(Mat& img, vector<Point>& trajectory, double start, const vector<Point>& smooth_center, vector<double>& trajectoryTimes, int bestIdx);
    void test_drawcross(Mat& img);
    void drawObject(Mat& image, const vector<Rect>& objects, const vector<Point>& objectcneter, const vector<Scalar>& mycolorvalue, int bestIdx);
    void draw_fps(Mat& img, double start);

};