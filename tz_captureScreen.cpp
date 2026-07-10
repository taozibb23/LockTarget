#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>

#include "tzbb.h"

using namespace cv;
using namespace std;

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
HDC hdcScreen = GetDC(NULL);					//获取屏幕dc上下文
HDC hdcMen = CreateCompatibleDC(hdcScreen);     //创建内存dc上下文

HDC hdcMem = CreateCompatibleDC(hdcScreen);
HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
SelectObject(hdcMem, hBitmap);