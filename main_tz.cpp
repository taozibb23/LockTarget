#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#include"tzbb.h"

extern ColorValue colorvalue;

const vector<Scalar> mycolorvalue{ {0,0,255}//Red
};
//vector<vector<int>> identifycolor{ {0, 100, 100, 10, 255, 255},     // 红色范围 1（色相 0-10）
//                                   {170, 100, 100, 180, 255, 255} };// 红色范围 2（色相 170-180）

Mat img;

//先用crakbark调一下，后面再看固定颜色

void main()
{
	VideoCapture cap(0);
	
	namedWindow("TrackBars", (640, 200));
	createTrackbar("Hue Min", "TrackBars", &colorvalue.hmin, 179);
	createTrackbar("Hue Max", "TrackBars", &colorvalue.hmax, 179);
	createTrackbar("Sat Min", "TrackBars", &colorvalue.smin, 255);
	createTrackbar("Sat Max", "TrackBars", &colorvalue.smax, 255);
	createTrackbar("Val Min", "TrackBars", &colorvalue.vmin, 255);
	createTrackbar("Val Max", "TrackBars", &colorvalue.vmax, 255);
	if (!cap.isOpened())
	{
		cout << "camera open failed" << endl;
		return;
	}
	vector<Point> trajectory;
	Armor detect;


	while (true)
	{
		double start = getTickCount();
		cap.read(img);
		
		Mat mask = detect.findmycolor(img);
		vector<Rect>objects = detect.getContours(mask);
		vector<Rect> AfterSmoothRect = detect.kalmanhandle(objects); //直接kalman滤波
		vector<Point> raw_center = detect.getObjectCenter(objects);  //用于后面kalman的中心点 
		vector<Point> objectcenter = detect.getObjectCenter(objects); //没任何滤波的中心点



		if (img.empty())
		{
			cout << "img is empty" << endl;
			break;
		}

		if (!objectcenter.empty())
		{
			trajectory.push_back(objectcenter[0]);
			for (int i = 1; i < trajectory.size(); i++)
			{
				line(img, trajectory[i - 1], trajectory[i], Scalar(0, 255, 0), 2);
			}
		}
		//识别红色方块

		//画框
		//显示中心点
		vector<Point> smooth_center = detect.getObjectCenter(AfterSmoothRect);
 		detect.drawObject(img, AfterSmoothRect, smooth_center, mycolorvalue);
		//计算帧率
		//fps 平滑 跟跟踪轨迹平滑一样
		double end = getTickCount();
		double time = (end - start) / getTickFrequency();
		double fps = 1.0 / time;
		string fpsText = "FPS= " + to_string(int(fps));
		putText(img, fpsText, Point(20, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

		imshow("mask", mask);
		imshow("video", img);
		waitKey(1);
	}

}