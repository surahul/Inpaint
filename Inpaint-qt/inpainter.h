#ifndef INPAINTER_H
#define INPAINTER_H

#include "opencv/cv.h"
#include "opencv/highgui.h"

class Inpainter
{
public:
    const static int DEFAULT_HALF_PATCH_WIDTH=3;
    const static int MODE_ADDITION=0;
    const static int MODE_MULTIPLICATION=1;
    const static int ERROR_INPUT_MAT_INVALID_TYPE=0;
    const static int ERROR_INPUT_MASK_INVALID_TYPE=1;
    const static int ERROR_MASK_INPUT_SIZE_MISMATCH=2;
    const static int ERROR_HALF_PATCH_WIDTH_ZERO=3;
    const static int CHECK_VALID=4;

    Inpainter(cv::Mat inputImage,cv::Mat mask,int halfPatchWidth=3,int mode=1);

    cv::Mat inputImage;
    cv::Mat mask,updatedMask;
    cv::Mat result;
    cv::Mat workImage;
    cv::Mat sourceRegion;
    cv::Mat targetRegion;
    cv::Mat originalSourceRegion;
    cv::Mat gradientX;
    cv::Mat gradientY;
    cv::Mat confidence;
    cv::Mat data;
    cv::Mat LAPLACIAN_KERNEL,NORMAL_KERNELX,NORMAL_KERNELY;
    cv::Point2i bestMatchUpperLeft,bestMatchLowerRight;
    std::vector<cv::Point> fillFront;
    std::vector<cv::Point2f> normals;
    int mode;
    int halfPatchWidth;
    int targetIndex;

    int checkValidInputs();
    void calculateGradients();
    void initializeMats();
    void computeFillFront();
    void computeConfidence();
    void computeData();
    void computeTarget();
    void computeBestPatch();
    void updateMats();
    bool checkEnd();
    void getPatch(cv::Point2i &centerPixel, cv::Point2i &upperLeft, cv::Point2i &lowerRight);
    void inpaint();


};


#endif // INPAINTER_H
