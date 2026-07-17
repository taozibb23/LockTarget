#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

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
	VideoCapture cap(1);
	
	//namedWindow("TrackBars", (640, 200));//调节trackbar调试找颜色的时候用其他时候不用
	//createTrackbar("Hue Min", "TrackBars", &colorvalue.hmin, 179);
	//createTrackbar("Hue Max", "TrackBars", &colorvalue.hmax, 179);
	//createTrackbar("Sat Min", "TrackBars", &colorvalue.smin, 255);
	//createTrackbar("Sat Max", "TrackBars", &colorvalue.smax, 255);
	//createTrackbar("Val Min", "TrackBars", &colorvalue.vmin, 255);
	//createTrackbar("Val Max", "TrackBars", &colorvalue.vmax, 255);
	if (!cap.isOpened())
	{
		cout << "camera open failed" << endl;
		return;
	}
	vector<Point> trajectory; //中心点连线
	vector<double> trajectoryTimes;// 储存时间的参数
	int stableCount = 0;
	Armor detect;
	PID pid_x(0.3, 0.0, 0.0);
	PID pid_y(0.3, 0.0, 0.0);

	while (true)
	{
		double start = getTickCount();
		cap.read(img);
		
		Mat mask = detect.findmycolor(img);//筛选颜色
		vector<Rect>objects = detect.getContours(mask); //变换成mask
		Rect SelectRect;
		int bestIdx = -1;
		if (!objects.empty())
		{
			Point ScreenCenter(img.cols / 2, img.rows / 2);
			double minDist = 9991314520;
			for (int i = 0; i < objects.size(); i++)
			{
				Point objCenter(objects[i].x + objects[i].width / 2, objects[i].y + objects[i].height / 2);
				double Dist = (ScreenCenter.x - objCenter.x)* (ScreenCenter.x - objCenter.x) + (ScreenCenter.y - objCenter.y)* (ScreenCenter.y - objCenter.y);

				if (Dist < minDist)
				{
					minDist = Dist;
					bestIdx = i;
				}
			}
		}
		vector<Rect> AfterSmooth_Rect;
		vector<Point> raw_center;
		vector<Point> objectcenter;
		vector<Point> smooth_center;
		
		if (bestIdx >= 0){
			vector<Rect> selected;
			selected.push_back(objects[bestIdx]);
			objects = selected;
		}
		AfterSmooth_Rect = detect.kalmanhandle(objects); //直接kalman滤波,处理出来的是矩形
		raw_center = detect.getObjectCenter(objects);  //第一次原始的中心点
		objectcenter = detect.getObjectCenter(objects); //没任何滤波的中心点
		smooth_center = detect.getObjectCenter(AfterSmooth_Rect); //用于画框的中心点，kalman处理过的计算中心点
		

		if (img.empty())
		{
			cout << "img is empty" << endl;
			break;
		}

		//if (!smooth_center.empty())
		//{
		//	POINT cursor;
		//	GetCursorPos(&cursor);

		//	double targetX = smooth_center[0].x;
		//	double targetY = smooth_center[0].y;
		//	
		//	SetCursorPos((int)targetX, (int)targetY);
		//	
		//
		//}

		if (!smooth_center.empty())
		{

			POINT cursor;
			GetCursorPos(&cursor);
			
			int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYSCREEN);

			double scaleX = (double)screenWidth / img.cols;
			double scaleY = (double)screenHeight / img.rows;//计算缩放比例，为了应对不同分辨率的屏幕

			double cursorImgX = cursor.x / scaleX;
			double cursorImgY = cursor.y / scaleY;

			Point refPoint(cursorImgX, cursorImgY);

			double targetX = smooth_center[0].x * scaleX;
			double targetY = smooth_center[0].y * scaleY;
			//PID对应的error  用于后面计算
			double errorX = targetX - cursorImgX;
			double errorY = targetY - cursorImgY;

			bool atEdge = (cursor.x <= 20 || cursor.x >= screenWidth - 20 || cursor.y <= 20 || cursor.y >= screenHeight - 20);

			if (atEdge) {

				pid_x.reset();
				pid_y.reset();

				SetCursorPos(targetX, targetY);
			}

			//知道了error就可以计算输出
			double moveX = pid_x.calculate(errorX);
			double moveY = pid_y.calculate(errorY);
			//-------------------------------------------
			double maxMove = 40.0;//最大移动像素,限制
			moveX = max(-maxMove, min(maxMove, moveX));//min作为clamp
			moveY = max(-maxMove, min(maxMove, moveY));
			//限幅
			//————————————————————————————————
			mouse_event(MOUSEEVENTF_MOVE, (int)moveX, (int)moveY, 0, 0);

			//------------------------------log显示-------------------------------------------
			string debugText = "Target :" + to_string((int)targetX) + "," + to_string((int)targetY);
			debugText += " Error : " + to_string((int)errorX) + "," + to_string((int)errorY);
			putText(img, debugText, Point(20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
			//------------------------------log-------------------------------------------
			
			cout << "target :" << targetX << ",'" << targetY << "  |  " << "cursor :" << cursor.x << "," << cursor.y << "  |  " << "error :" << errorX << "," << errorY << "  |  " << "move :" << moveX << "," << moveY << endl;
		   //打印log
         // ------------------------------控制鼠标点击--------------------------
			double distance = sqrt(errorX * errorX + errorY * errorY);
			int click_threshold = 5;
			double aim_distance = 10.0;
			if (distance <= aim_distance)
			{
				stableCount++;
				if (stableCount >= click_threshold) {
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					stableCount = 0;
				}
			}
		
		}
		else {
			pid_x.reset();
			pid_y.reset();
		}
		
		double now = getTickCount() / getTickFrequency(); //当前时间(秒)
		double start_sec = start / getTickFrequency(); //记录时间戳
		if (!smooth_center.empty())
		{
			trajectory.push_back(smooth_center[0]);
			trajectoryTimes.push_back(start_sec);//储存的是秒
		}
		while (!trajectoryTimes.empty() && now - trajectoryTimes[0] > 1.0) 
		{
			trajectory.erase(trajectory.begin());
			trajectoryTimes.erase(trajectoryTimes.begin());

		}
		for (int i = 1; i < trajectory.size(); i++)
		{
			line(img, trajectory[i - 1], trajectory[i], Scalar(0, 255, 0), 2,LINE_AA);
		}
		
		

		//画框
		//显示中心点
		
 		detect.drawObject(img, AfterSmooth_Rect, smooth_center, mycolorvalue);
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