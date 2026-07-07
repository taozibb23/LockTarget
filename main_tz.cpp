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

	Kalman1D kfX(0.5, 5.0, 320, 100);  //   X
	Kalman1D kfY(0.5, 5.0, 240, 100);  //   Y
	// q=0.5(目标运动会变化), r=5(检测噪声), 初始猜320, 初始p=100
	while (true)
	{
		double start = getTickCount();
		cap.read(img);
		
		Mat mask = detect.findmycolor(img);
		vector<Rect>objects = detect.getContours(mask);
		vector<Rect> smoothobjects = detect.smoothhandle(objects);
		vector<Point> raw_center = detect.getObjectCenter(objects);   //新加的kalman
		vector<Point> objectcenter = detect.getObjectCenter(objects); //原来的

		kfX.predict();
		kfY.predict();
		Point kalman_center;
		if (!raw_center.empty()) {
			double zx = raw_center[0].x;
			double zy = raw_center[0].y;

			kfX.update(zx);
			kfY.update(zy);

			kalman_center = Point(kfX.state(), kfY.state());
		}
		else {
			kalman_center = Point(kfX.state(), kfY.state());
		}


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
		detect.drawObject(img, smoothobjects, objectcenter, mycolorvalue);

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