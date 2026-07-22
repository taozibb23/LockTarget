#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <algorithm> //因为上面禁用了宏 min 和 max 这里可以重新开始使用min和max

using namespace cv;
using namespace std;

#include"tzbb.h"
int Locktarget::claculatebestIdx(vector<Point>& smooth_center, int& lockedIdx, double cursorImgX, double cursorImgY)
{
	//-------------------------------目标锁定代码--------------------------------
	int bestIdx = -1;//用于筛选最近距离的索引
	double minDist = 999999999;
	int result;
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
	result = bestIdx;
	return result;
}

void Locktarget::calculatemove(Mat& img, double targetX, double targetY, POINT cursor, PID& pid_x, PID& pid_y, int& stableCount)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

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




