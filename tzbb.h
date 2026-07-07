#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class Armor
{
public:

	vector<Rect> prev_objects;

    void drawObject(Mat& image, const vector<Rect>& objects, const vector<Point>& objectcneter, const vector<Scalar>& mycolorvalue);
	Mat findmycolor(const Mat& image);
	vector<Rect> getContours(const Mat& image);
	vector<Rect> smoothhandle(const vector<Rect>& objects);
	vector<Point> getObjectCenter(const vector<Rect>& objects);
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
class Kalman1D {
public:
	double x; //guess state
	double p; //guess uncertenty
	double q; //process uncententy
	double r; //measurement uncertenty
	
	Kalman1D(double q_, double r_, double init_x, double init_p)
		: q(q_), r(r_), x(init_x), p(init_p) {}
    
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