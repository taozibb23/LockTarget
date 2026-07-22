#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "display.h"

using namespace std;
using namespace cv;

void Display::displaytrajectory(Mat& img,vector<Point>& trajectory, double start,const vector<Point>& smooth_center,vector<double>&trajectoryTimes,int bestIdx)
{
	double now = getTickCount() / getTickFrequency(); //当前时间(秒)
	double start_sec = start / getTickFrequency(); //记录时间戳

	trajectory.push_back(smooth_center[bestIdx]);
	trajectoryTimes.push_back(start_sec);//储存的是秒

	while (!trajectoryTimes.empty() && now - trajectoryTimes[0] > 1.0)
	{

		trajectory.erase(trajectory.begin());
		trajectoryTimes.erase(trajectoryTimes.begin());

	}

	for (int i = 1; i < trajectory.size(); i++)
	{
		line(img, trajectory[i - 1], trajectory[i], Scalar(0, 255, 0), 2, LINE_AA);
	}
	//记录时间戳并且画出轨迹
}

void Display::test_drawcross(Mat& img)
{

	Point center_test(img.cols / 2, img.rows / 2);
	line(img, Point(center_test.x - 20, center_test.y), Point(center_test.x + 20, center_test.y), Scalar(0, 255, 0), 1);
	line(img, Point(center_test.x, center_test.y - 20), Point(center_test.x, center_test.y + 20), Scalar(0, 255, 0), 1);

}

void Display::drawObject(Mat& image, const vector<Rect>& smooth_Rect, const vector<Point>& smooth_cneter, const vector<Scalar>& mycolorvalue,int bestIdx)
{

	for (int i = 0; i < smooth_Rect.size(); i++)
	{
		rectangle(image, smooth_Rect[bestIdx].tl(), smooth_Rect[bestIdx].br(), mycolorvalue[0], 5);
		circle(image, smooth_cneter[bestIdx], 5, mycolorvalue[0], 5, FILLED);
	}

}

void Display::draw_fps(Mat& img,double start)
{

	double end = getTickCount();
	double time = (end - start) / getTickFrequency();
	double fps = 1.0 / time;
	string fpsText = "FPS= " + to_string(int(fps));
	putText(img, fpsText, Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

}