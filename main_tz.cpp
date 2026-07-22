#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

#include"tzbb.h"
#include"display.h"

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
	int lockedIdx = -1;//锁定的索引数（值）  （保证后面下一帧的时候还记得上一帧率）
	Armor detect;
	Locktarget locktarget;
	Display display;
	PID pid_x(0.3, 0.0, 0.0);
	PID pid_y(0.3, 0.0, 0.0);

	while (true)
	{
		double start = getTickCount();
		cap.read(img);
		//---------------------------------------------------------------------
		Mat mask = detect.findmycolor(img);//筛选颜色 
		vector<Rect>objects = detect.getContours(mask); //变换成mask 
		vector<Point> raw_center = detect.getObjectCenter(objects);  //第一次原始的中心点   原始（点） 
		vector<Rect> smooth_Rect = detect.kalmanhandle(objects); //直接kalman滤波,处理出来的是矩形   kalman(矩形)  
		vector<Point> smooth_center = detect.getObjectCenter(smooth_Rect); //用于画框的中心点，kalman处理过的计算中心点  


		if (!smooth_Rect.empty() && !smooth_center.empty())
		{
			//-------------------------------------------

			Rect SelectRect;
			POINT cursor;
			GetCursorPos(&cursor);
			Point ScreenCenter(img.cols / 2, img.rows / 2);

			if (img.empty())
			{
				cout << "img is empty" << endl;
				break;
			}


			//坐标映射
			int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYSCREEN);

			double scaleX = (double)screenWidth / img.cols;
			double scaleY = (double)screenHeight / img.rows;//计算缩放比例，为了应对不同分辨率的屏幕

			double cursorImgX = cursor.x / scaleX;
			double cursorImgY = cursor.y / scaleY;

			Point refPoint(cursorImgX, cursorImgY);

			//-------------------------------目标锁定代码--------------------------------
			
			int bestIdx = locktarget.claculatebestIdx(smooth_center, lockedIdx, cursorImgX, cursorImgY);

			//---------------------------锁定目标————————————-- 算是插入了，临时插入
			if (bestIdx >= 0)
			{
				double targetX = smooth_center[bestIdx].x * scaleX;
				double targetY = smooth_center[bestIdx].y * scaleY;
				
				locktarget.calculatemove(img, targetX, targetY, cursor, pid_x, pid_y, stableCount);

			}
		 
		 //----------------------------------draw 绘制----------------------------------------
			display.displaytrajectory(img, trajectory, start, smooth_center, trajectoryTimes, bestIdx);
			//-------------------测试映射（用中心点十字架）-------------------
			display.test_drawcross(img);
			//-------------------测试映射（中心点十字架）
		    //画框
		    //显示中心点
			display.drawObject(img, smooth_Rect, smooth_center, mycolorvalue, bestIdx);
			//计算帧率
			//fps 平滑 跟跟踪轨迹平滑一样
			display.draw_fps(img, start);
		
		}
        //--------------------------bestIdx小于0
		else
		{
			pid_x.reset();
			pid_y.reset();

			display.draw_fps(img, start);
		}

	   //--------------------------
		imshow("mask", mask);
		imshow("video", img);
		waitKey(1);
		
	}
}