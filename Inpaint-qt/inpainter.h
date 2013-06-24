#ifndef INPAINTER_H
#define INPAINTER_H

#include "gradientcalculator.h"

class inpainter
{
public:

    bool inpaint(cv::Mat &source,cv::Mat &result,cv::Mat &mask,bool quickInpaint);
    void getPatch(cv::Point2f &centerPixel,cv::Point2f &upperLeft,cv::Point2f &lowerRight);
    cv::Mat originalImage;
    cv::Mat workImage,grayImage;
    cv::Mat mask;
    cv::Mat source,originalSource;
    cv::Mat target;
    cv::Mat gradientX;
    cv::Mat gradientY;
    cv::Mat confidence;
    cv::Mat data;
    int halfPatchWidth=0;


};

#endif // INPAINTER_H
