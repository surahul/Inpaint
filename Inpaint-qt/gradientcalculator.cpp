/*
 * Author Rahul Verma
 * Date   26-04-2013
 */

#include "gradientcalculator.h"


GradientCalculator::GradientCalculator()
{

}

cv::Mat GradientCalculator::getGradX(){
    return this->gradX;
}

cv::Mat GradientCalculator::getGradY(){
    return this->gradY;
}


void GradientCalculator::calculateGradient(cv::Mat &src){
    this->gradX=cv::Mat(src.rows,src.cols,CV_32F,cv::Scalar::all(0));
    this->gradY=gradX.clone();

    cv::Vec3b pixel1;
    cv::Vec3b pixel0;
    cv::Vec3f pixelDiff;


    int x,y;
    if(src.rows>1){

        for( x=0 ; x < src.cols ; x++ ){
            pixel1=src.at<cv::Vec3b>(1,x);
            pixel0=src.at<cv::Vec3b>(0,x);
            pixelDiff=pixel1-pixel0;
            gradX.at<float>(0,x)=-(pixelDiff[0]+pixelDiff[1]+pixelDiff[2])/(3*255.0);

            pixel1=src.at<cv::Vec3b>(src.rows-1,x);
            pixel0=src.at<cv::Vec3b>(src.rows-2,x);
            pixelDiff=pixel1-pixel0;
            gradX.at<float>(src.rows-1,x)=-(pixelDiff[0]+pixelDiff[1]+pixelDiff[2])/(3*255.0);


        }

    }

    if(src.rows>2){
        for(y=1 ; y < src.rows-1; y++){
             for(x=0 ; x < src.cols ; x++){
                 pixel1=src.at<cv::Vec3b>(y+1,x);
                 pixel0=src.at<cv::Vec3b>(y-1,x);
                 pixelDiff=pixel1-pixel0;
                 gradX.at<float>(y,x)=-(pixelDiff[0]+pixelDiff[1]+pixelDiff[2])/(3*255.0);

             }
        }

    }

    if(src.cols>1){

        for( y=0 ; y < src.rows ; y++ ){
            pixel1=src.at<cv::Vec3b>(y,1);
            pixel0=src.at<cv::Vec3b>(y,0);
            pixelDiff=pixel1-pixel0;
            gradY.at<float>(y,0)=-(pixelDiff[0]+pixelDiff[1]+pixelDiff[2])/(3*255.0);

            pixel1=src.at<cv::Vec3b>(y,src.cols-1);
            pixel0=src.at<cv::Vec3b>(y,src.cols-2);
            pixelDiff=pixel1-pixel0;
            gradY.at<float>(y,src.cols-1)=-(pixelDiff[0]+pixelDiff[1]+pixelDiff[2])/(3*255.0);


        }

    }

    if(src.cols>2){
        for(x=1 ; x < src.cols-1; x++){
             for(y=0 ; y < src.rows ; y++){
                 pixel1=src.at<cv::Vec3b>(y,x+1);
                 pixel0=src.at<cv::Vec3b>(y,x-1);
                 pixelDiff=pixel1-pixel0;
                 gradY.at<float>(y,x)=-(pixelDiff[0]+pixelDiff[1]+pixelDiff[2])/(3*255.0);

             }
        }

    }


}





