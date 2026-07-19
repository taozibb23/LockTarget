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
	int lockedIdx = -1;//锁定的索引数（值）  （保证后面下一帧的时候还记得上一帧率）
	Armor detect;
	
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


			//-------------------------------------------
	
		 // if (bestIdx >= 0)
		 // {
			//vector<Point> selected_center;
			//selected_center.push_back(smooth_center[bestIdx]);
			//smooth_center = selected_center;
	  //   	//-----------------同步更新center的时候也要更新rect-------------------------
			//vector<Rect> selected_Rect;
			//selected_Rect.push_back(smooth_Rect[bestIdx]);
			//smooth_Rect = selected_Rect;

		 // }
		  //计算最近距离得出一个smooth

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
			int bestIdx = -1;//用于筛选最近距离的索引
			double minDist = 999999999;

			bool needReselect = true;
			if (lockedIdx >= 0 && lockedIdx < smooth_center.size())
			{


				double dx = smooth_center[lockedIdx].x - cursorImgX;
				double dy = smooth_center[lockedIdx].y - cursorImgY;

				double disToLocked = sqrt(dx * dx + dy * dy);
				if (disToLocked <= 50.0)
				{
					needReselect = false;
					bestIdx = lockedIdx;
				}
			}

			if (needReselect)
			{
				for (int i = 0; i < smooth_center.size(); i++)
				{
					double Dist = ((cursorImgX - smooth_center[i].x) * (cursorImgX - smooth_center[i].x)
						+ (cursorImgY - smooth_center[i].y) * (cursorImgY - smooth_center[i].y));

					if (Dist <= minDist)
					{
						minDist = Dist;
						bestIdx = i;
					}
				}
				if (bestIdx >= 0)
				{
					lockedIdx = bestIdx;
				}
			}
			//---------------------------锁定目标————————————-- 算是插入了，临时插入
            if (bestIdx >= 0)
			{
		     	double targetX = smooth_center[bestIdx].x * scaleX;
			    double targetY = smooth_center[bestIdx].y * scaleY;
			    //PID对应的error  用于后面计算
			    double errorX = targetX - cursor.x;
			    double errorY = targetY - cursor.y;

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
		 

         // ------------------------------控制鼠标点击------------------------
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
		 //---------------------------------------------------------------
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
		   //-------------------测试映射（用中心点十字架）-------------------
		  Point center_test(img.cols / 2,img.rows / 2);
	     	line(img, Point(center_test.x - 20, center_test.y), Point(center_test.x + 20, center_test.y), Scalar(0, 255, 0), 1);
		   line(img, Point(center_test.x, center_test.y - 20), Point(center_test.x, center_test.y + 20), Scalar(0, 255, 0), 1);
	    	//-------------------测试映射（中心点十字架）
		  //画框
		 //显示中心点
		
 	    	detect.drawObject(img, smooth_Rect, smooth_center, mycolorvalue);
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