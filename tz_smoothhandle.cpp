#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#include"tzbb.h"

vector<Rect> Armor::smoothhandle(const vector<Rect>& objects)
{
	Rect oldRect;
	Rect newRect;
	float alpha = 0.7;
	vector<Rect> result = objects;
	if (objects.size() == prev_objects.size())
	{
		for (int i = 0; i < objects.size(); i++)
		{
			oldRect = prev_objects[i];
			newRect = objects[i];

			int x = (int)(alpha * oldRect.x + (1 - alpha) * newRect.x);
			int y = (int)(alpha * oldRect.y + (1 - alpha) * newRect.y);
			int width = (int)(alpha * oldRect.width + (1 - alpha) * newRect.width);
			int height = (int)(alpha * oldRect.height + (1 - alpha) * newRect.height);

			result[i] = Rect(x, y, width, height);
		}

	}
	Armor::prev_objects = result;
	return result;
}