#pragma once

#include <opencv2/opencv.hpp>
class __declspec(dllimport) HandInterface {
public:
	virtual int run(cv::Mat &&src) = 0;
	virtual void setDraw(bool draw) = 0;
};

__declspec(dllimport) HandInterface *factory_function();
