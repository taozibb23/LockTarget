#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;



class Kalman1D {
public:
	double x; //guess state
	double p; //guess uncertenty
	double q; //process uncententy
	double r; //measurement uncertenty

	Kalman1D() : x(320), p(100), q(0.5), r(5) {}// q=0.5(目标运动会变化), r=5(检测噪声), 初始猜320, 初始p=100

	Kalman1D(double q_, double r_, double init_x, double init_p)
		: q(q_), r(r_), x(init_x), p(init_p) {
	}

	void predict() {
		p += q;
	}
	void update(double z) {
		double k = p / (p + r);
		x = x + k * (z - x);
		p = (1 - k) * p;
	}

	double state() const { return x; }


};


class Armor
{
public:
	vector<Rect> prev_objects; //给smooth使用处理之后再用kalman处理
	vector<Rect> result;

    void drawObject(Mat& image, const vector<Rect>& objects, const vector<Point>& objectcneter, const vector<Scalar>& mycolorvalue);
	Mat findmycolor(const Mat& image);
	/*vector<Rect> smoothhandle(const vector<Rect>& objects);*///已经不需要了，直接用kalman滤波 
	vector<Rect> getContours(const Mat& image);
	vector<Point> getObjectCenter(const vector<Rect>& objects);

public:
	vector<Rect> kalmanhandle(const vector<Rect>& objects);

	private:
	Kalman1D kfX;
	Kalman1D kfY;
};



class ColorValue
{
public:
	int hmin , smin , vmin ;
	int hmax , smax , vmax ;

	ColorValue()
	{
		hmin = 0;
		smin = 0;
		vmin = 0;
		hmax = 179;
		smax = 255;
		vmax = 255;
	}
};


//
//class Kalmanvalue {
//
//private:
//	Kalman1D kfX;
//	Kalman1D kfY;
//
//};
//
//class Tool {
//
//public:
//
//	Point CalculateCenter(const vector<Rect>& objects) {
//		if (objects.empty()) {
//			return Point(-1, -1);
//		}
//		else {
//			Point center = Point(objects[0].x + objects[0].width / 2, objects[0].y + objects[0].height / 2);
//			return center;
//		}
//	}
//
//};