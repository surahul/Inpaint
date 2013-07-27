/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2008-2012, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

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

    Inpainter(cv::Mat inputImage,cv::Mat mask,int halfPatchWidth=4,int mode=1);

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
