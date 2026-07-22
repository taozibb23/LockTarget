#define NOMINMAX
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "tzbb.h"

using namespace std;
using namespace cv;

ColorValue colorvalue;

Mat Armor::findmycolor(const Mat& image)
{
	Mat imgHSV;
	Mat mask;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	cvtColor(image, imgHSV, COLOR_BGR2HSV);
	//for (int i = 0; i < identifycolor.size(); i++)
	//{
	//Scalar lower(colorvalue.hmin, colorvalue.smin, colorvalue.vmin);//用trackbar调节
	//Scalar upper(colorvalue.hmax, colorvalue.smax, colorvalue.vmax);

	Scalar lower(colorvalue.bluehmin, colorvalue.bluesmin, colorvalue.bluevmin);
	Scalar upper(colorvalue.bluehmax, colorvalue.bluesmax, colorvalue.bluevmax);//aimlabs蓝色小球

	inRange(imgHSV, lower, upper, mask);//筛选目标颜色输出mask
	morphologyEx(mask, mask, MORPH_OPEN, kernel);

	//}
	return mask;
}



vector<Rect> Armor::getContours(const Mat& image)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<Rect> objects;
	Mat imageblur;
	float aspectratiomin = 0.8;
	float aspectratiomax = 1.2;
	double maxArea = 0;
	int maxIndex = -1;
	GaussianBlur(image, imageblur, Size(5, 5), 0);
	findContours(imageblur, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());

	for (int i = 0; i < contours.size(); i++) //遍历轮廓
	{
		float peri = arcLength(contours[i], true);
		approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
		double area = contourArea(contours[i]);
		if (area > maxArea)//判断最大的识别面积
		{
			maxArea = area;
			maxIndex = i;
			//draw contours
			Rect box = boundingRect(conPoly[maxIndex]);//box 用来算比例排除，真正数据用objects存储
			double aspectratio = (float)box.width / (float)box.height;
			double rectarea = box.width * box.height;
			if (aspectratio > aspectratiomin && aspectratio < aspectratiomax)
			{
				if (rectarea > 1000) //筛选小面积
				{
					objects.push_back(box);

				}

			}
		}

	}
	return objects;
}


vector<Point> Armor::getObjectCenter(const vector<Rect>& objects)
{

	vector<Point> objectcenter;

	for (int i = 0; i < objects.size(); i++)
	{
		objectcenter.push_back(Point(objects[i].x + objects[i].width / 2, objects[i].y + objects[i].height / 2));
	}
	return objectcenter;
}


void Armor::drawObject(Mat& image, const vector<Rect>& objects, const vector<Point>& objectcneter, const vector<Scalar>& mycolorvalue)
{
	for (int i = 0; i < objects.size(); i++)
	{
		rectangle(image, objects[i].tl(), objects[i].br(), mycolorvalue[0], 5);
		circle(image, objectcneter[i], 5, mycolorvalue[0], 5, FILLED);
	}

}

//if (!raw_center.empty()) {
//	double zx = raw_center[0].x;
//	double zy = raw_center[0].y;
//
//	kfX.update(zx);
//	kfY.update(zy);
//
//	kalman_center = Point(kfX.state(), kfY.state());
//}
//else {
//	kalman_center = Point(kfX.state(), kfY.state());
//}

vector<Rect> Armor::kalmanhandle(const vector<Rect>& objects)
{
	vector<Rect>result;
	if (objects.empty())
	{
		return result;
	}
	else {
		for (int i = 0; i < objects.size(); i++) {
			Point kalman_center = Point(objects[i].x + objects[i].width / 2, objects[i].y + objects[i].height / 2);


			kfX.predict();
			kfY.predict();

			kfX.update(kalman_center.x);
			kfY.update(kalman_center.y);

			double smoothkalman_x = kfX.state();
			double smoothkalman_y = kfY.state();

			Rect kalmanRect = Rect(smoothkalman_x - objects[i].width / 2,
				smoothkalman_y - objects[i].height / 2,
				objects[i].width,
				objects[i].height);
			result.push_back(kalmanRect);
		}
		return result;
	}
}



//vector<Rect> Armor::smoothhandle(const vector<Rect>& objects)
//{
//	Rect oldRect;
//	Rect newRect;
//	float alpha = 0.7;
//	vector<Rect> result = objects;
//	if (objects.size() == prev_objects.size())
//	{
//		for (int i = 0; i < objects.size(); i++)
//		{
//			oldRect = prev_objects[i];
//			newRect = objects[i];
//
//			int x = (int)(alpha * oldRect.x + (1 - alpha) * newRect.x);
//			int y = (int)(alpha * oldRect.y + (1 - alpha) * newRect.y);
//			int width = (int)(alpha * oldRect.width + (1 - alpha) * newRect.width);
//			int height = (int)(alpha * oldRect.height + (1 - alpha) * newRect.height);
//
//			result[i] = Rect(x, y, width, height);
//		}
//
//	}
//	Armor::prev_objects = result;
//	return result;
//}