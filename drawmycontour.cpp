#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;
#include"tzbb.h"

void Armor::drawObject(Mat& image, const vector<Rect>& objects, const vector<Point>& objectcneter, const vector<Scalar>& mycolorvalue)
{
	for (int i = 0; i < objects.size(); i++)
	{
		rectangle(image, objects[i].tl(), objects[i].br(), mycolorvalue[0], 5);
		circle(image, objectcneter[i], 5, mycolorvalue[0], 5, FILLED);
	}

}