/*Author : Rahul Verma  
 *Date 22/06/13
 */

#include "inpainter.h"

bool inpainter::inpaint(cv::Mat &src,cv::Mat &result,cv::Mat &msk,bool quickInpaint){
    this->originalImage=src.clone();
    this->workImage=originalImage.clone();
    cv::cvtColor(workImage,grayImage,CV_BGR2GRAY);
    this->mask=msk.clone();
    result.create(originalImage.size(),originalImage.type());

    //if(!CV_ARE_SIZES_EQ(originalImage,mask)||!CV_ARE_SIZES_EQ(originalImage,result))
    //   return false;
    if(originalImage.type()!=CV_8UC3||mask.type()!=CV_8UC1)
        return false;


    GradientCalculator gc;
    gc.calculateGradient(originalImage);
    this->gradientX=gc.getGradX();
    this->gradientY=gc.getGradY();


    //set initial confidence values, 1 for source region pixels and 0 for target region pixels
    cv::threshold(this->mask,this->confidence,10,255,CV_THRESH_BINARY);
    cv::threshold(confidence,confidence,2,1,CV_THRESH_BINARY_INV);
    confidence.convertTo(confidence,CV_32F);

    //set initial binarySource values, 1 for source region pixels and 0 for target region pixels
    this->source=confidence.clone();
    this->source.convertTo(source,CV_8U);
    this->originalSource=source.clone();

    //set initiall binaryTarget values, 1 for target region pixels and 0 for source region pixels
    cv::threshold(mask,this->target,10,255,CV_THRESH_BINARY);
    cv::threshold(target,target,2,1,CV_THRESH_BINARY);
    target.convertTo(target,CV_32F);

    //set initial data values for every pixel equal to -0.1;
    data=cv::Mat(originalImage.rows,originalImage.cols,CV_32F,cv::Scalar::all(-0.1));

    cv::Mat lapKern=cv::Mat::ones(3,3,CV_32F),normKernX=cv::Mat(1,3,CV_32F),normKernY=cv::Mat(3,1,CV_32F);
    lapKern.at<float>(1,1)=-8;
    normKernX.at<float>(0,1)=0;
    normKernX.at<float>(0,0)=-1;
    normKernX.at<float>(0,2)=1;
    normKernY.at<float>(1,0)=0;
    normKernY.at<float>(0,0)=-1;
    normKernY.at<float>(2,0)=1;


    cv::Mat boundryMat,normalMatX,normalMatY;
    std::vector<cv::Point2f> fillFront,unitNormals;

    int x,y,i,x2,y2,targetIndex,currentPatchWidth,currentPatchHeight,pixeli1,pixeli2;
    float dx,dy,tempF,data,maxPriority;

    double minimumError,error;
    cv::Vec3d pixel1,pixel2;
    cv::Point2f a,b,normal,currentUpperLeft,currentLowerRight,bestMatchUpperLeft,bestMatchLowerRight;

    bool stay=true,skipPatch;
    while(stay){


        cv::filter2D(target,boundryMat,CV_32F,lapKern);
        cv::filter2D(target,normalMatX,CV_32F,normKernX);
        cv::filter2D(target,normalMatY,CV_32F,normKernY);



        fillFront.clear();
        unitNormals.clear();

        for(x=0;x<boundryMat.cols;x++){
            for(y=0;y<boundryMat.rows;y++){
                if(boundryMat.at<float>(y,x)>0){
                    fillFront.push_back(cv::Point2f(x,y));
                    dx=normalMatX.at<float>(y,x);
                    dy=normalMatY.at<float>(y,x);
                    normal=cv::Point2f(dy,-dx);
                    tempF=std::sqrt((normal.x*normal.x)+(normal.y*normal.y));
                    if(tempF!=0){

                    normal.x=normal.x/tempF;
                    normal.y=normal.y/tempF;

                    }
                    unitNormals.push_back(normal);


                }
            }
        }

        std::cout<<std::endl<<fillFront.size();


        for(i=0;i<fillFront.size();i++){
            tempF=0;

            getPatch(fillFront.at(i),a,b);
            for(x=a.x;x<=b.x;x++){
                for(y=a.y;y<=b.y;y++){
                    if(target.at<float>(y,x)==0)
                        tempF+=confidence.at<float>(y,x);
                }
            }
            y2=((cv::Point2f)fillFront.at(i)).y;
            x2=((cv::Point2f)fillFront.at(i)).x;
            confidence.at<float>(y2,x2)=tempF/((b.x-a.x+1)*(b.y-a.y+1));

        }


        maxPriority=-1;
        targetIndex=-1;
        for(i=0;i<fillFront.size();i++){
            y=((cv::Point2f)fillFront.at(i)).y;
            x=((cv::Point2f)fillFront.at(i)).x;

            data=std::fabs(gradientY.at<float>(y,x)*((cv::Point2f)unitNormals.at(i)).x-gradientX.at<float>(y,x)*((cv::Point2f)unitNormals.at(i)).y)+.001;
            //std::cout<<std::endl<<"data "<<data<<std::endl<<"conf "<<confidence.at<float>(y,x)<<std::endl<<"normal "<<unitNormals.at(i);

            tempF=data*confidence.at<float>(y,x);
            if(tempF>maxPriority){
                maxPriority=tempF;
                targetIndex=i;
            }

        }

        if(targetIndex==-1){
        targetIndex=0;
        }
        minimumError=9999999999999999;


        getPatch(fillFront.at(targetIndex),a,b);
        currentPatchWidth=b.x-a.x+1;
        currentPatchHeight=b.y-a.y+1;
        for(x=0;x<workImage.cols-(currentPatchWidth);x++){
            for(y=0;y<workImage.rows-(currentPatchHeight);y++){
                currentUpperLeft=cv::Point2f(x,y);
                currentLowerRight=cv::Point2f(x+currentPatchWidth-1,y+currentPatchHeight-1);
                skipPatch=false;
                for(x2=0;x2<currentPatchWidth;x2++){
                    for(y2=0;y2<currentPatchHeight;y2++){
                        if(originalSource.at<uchar>(currentUpperLeft.y+y2,currentUpperLeft.x+x2)==0){
                            skipPatch=true;

                            break;
                        }
                }
                }
                if(skipPatch)
                    continue;
                error=0;

                for(x2=0;x2<currentPatchWidth;x2++){
                    for(y2=0;y2<currentPatchHeight;y2++){
                        if(source.at<uchar>(a.y+y2,a.x+x2)==0)
                            continue;
                          pixel1=workImage.at<cv::Vec3b>(currentUpperLeft.y+y2,currentUpperLeft.x+x2);
                          pixel2=workImage.at<cv::Vec3b>(a.y+y2,a.x+x2);
                          pixel1=pixel1-pixel2;
                          error+=(pixel1[0]*pixel1[0])+(pixel1[1]*pixel1[1])+(pixel1[2]*pixel1[2]);

//                        pixeli1=grayImage.at<uchar>(currentUpperLeft.y+y2,currentUpperLeft.x+x2);
//                        pixeli2=grayImage.at<uchar>(a.y+y2,a.x+x2);
//                        error+=(pixeli1-pixeli2)*(pixeli1-pixeli2);


                     }
                }
                if(error<minimumError){
                    minimumError=error;
                    bestMatchUpperLeft=currentUpperLeft;
                    bestMatchLowerRight=currentLowerRight;
                }


            }
        }

        y2=((cv::Point2f)fillFront.at(targetIndex)).y;
        x2=((cv::Point2f)fillFront.at(targetIndex)).x;

        for(x=0;x<currentPatchWidth;x++){
            for(y=0;y<currentPatchHeight;y++){
                if(source.at<uchar>(a.y+y,a.x+x)==0){
                    workImage.at<cv::Vec3b>(a.y+y,a.x+x)=workImage.at<cv::Vec3b>(bestMatchUpperLeft.y+y,bestMatchUpperLeft.x+x);
                    grayImage.at<uchar>(a.y+y,a.x+x)=grayImage.at<uchar>(bestMatchUpperLeft.y+y,bestMatchUpperLeft.x+x);
                    confidence.at<float>(a.y+y,a.x+x)=confidence.at<float>(y2,x2);
                    source.at<uchar>(a.y+y,a.x+x)=1;
                    target.at<float>(a.y+y,a.x+x)=0;
                }
            }
        }
        stay=false;
        for(x=0;x<source.cols;x++){
            for(y=0;y<source.rows;y++){
                if(source.at<uchar>(y,x)==0){
                    stay=true;
                    break;
                }
            }

        }



















    }

    std::cout<<"done";



}

void inpainter::getPatch(cv::Point2f &centerPixel, cv::Point2f &upperLeft, cv::Point2f &lowerRight){
    if(halfPatchWidth==0||halfPatchWidth%2==0)
        halfPatchWidth=4;

    int x,y;
    x=centerPixel.x;
    y=centerPixel.y;

    int minX=std::max(x-halfPatchWidth,0);
    int maxX=std::min(x+halfPatchWidth,workImage.cols-1);
    int minY=std::max(y-halfPatchWidth,0);
    int maxY=std::min(y+halfPatchWidth,workImage.rows-1);

    upperLeft.x=minX;
    upperLeft.y=minY;

    lowerRight.x=maxX;
    lowerRight.y=maxY;
}
