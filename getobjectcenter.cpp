#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;
#include"tzbb.h"
vector<Point> Armor::getObjectCenter(const vector<Rect>& objects)
{
	
	vector<Point> objectcenter;
	
	for (int i = 0; i < objects.size(); i++)
	{
		objectcenter.push_back(Point(objects[i].x + objects[i].width / 2, objects[i].y + objects[i].height / 2));
	}
	return objectcenter;
}
