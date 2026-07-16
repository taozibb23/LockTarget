#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;
#include"tzbb.h"
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