#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#include"tzbb.h"
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