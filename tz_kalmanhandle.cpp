#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#include"tzbb.h"


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
	if(objects.empty())
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