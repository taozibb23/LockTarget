#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
//img是cv转成的图片
//screen是屏幕尺寸，也就是如果是全屏状态下面就是没转的图片尺寸
//理解为opencv的mat尺寸是把原来的图片压缩了
int main() {
	//把视频转化为opencv之后的 图像mat
	int img_cols = 640;
	int img_rows = 480;

	//实际录频的尺寸
	int screenWidth = 1920;
	int screenHeight = 1080;

	//算屏幕尺寸和cv的mat的比例后面用于缩放
	double scaleX = (double)screenWidth / img_cols;
	double scaleY = (double)screenHeight / img_rows;

	//cv的屏幕中心位置
	double target_img_x = 320;
	double target_img_y = 240;

	//映射
	double target_screen_x = target_img_x * scaleX;
	double target_screen_y = target_img_y * scaleY;

	//鼠标在屏幕上的位置（屏幕）
	double cursor_screen_x = 1000;
	double cursor_screen_y = 600;

	//
	double error_x = target_screen_x - cursor_screen_x;
	double error_y = target_screen_y - cursor_screen_y;

	cout << "正常误差 ：" << error_x << ", " << error_y << endl;

	//
	double cursor_img_x = cursor_screen_x / scaleX;
	double cursor_img_y = cursor_screen_y / scaleY;
	double error_bad_x = target_screen_x - cursor_img_x;
	double error_bad_y = target_screen_y - cursor_img_y;

	cout << "错误误差： " << error_bad_x << ", " << error_bad_y << endl;

	return 0;
      
}