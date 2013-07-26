#include "inpainter.h"

Inpainter::Inpainter(cv::Mat inputImage,cv::Mat mask,int halfPatchWidth,int mode){
    this->inputImage=inputImage.clone();
    this->mask=mask.clone();
    this->updatedMask=mask.clone();
    this->workImage=inputImage.clone();
    this->result.create(inputImage.size(),inputImage.type());
    this->mode=mode;
    this->halfPatchWidth=halfPatchWidth;
}

int Inpainter::checkValidInputs(){
    if(this->inputImage.type()!=CV_8UC3)
        return ERROR_INPUT_MAT_INVALID_TYPE;
    if(this->mask.type()!=CV_8UC1)
        return ERROR_INPUT_MASK_INVALID_TYPE;
    if(!CV_ARE_SIZES_EQ(&mask,&inputImage))
        return ERROR_MASK_INPUT_SIZE_MISMATCH;
    if(halfPatchWidth==0)
        return ERROR_HALF_PATCH_WIDTH_ZERO;
    return CHECK_VALID;
}


void Inpainter::inpaint(){

    cv::namedWindow("updatedMask");
    cv::namedWindow("inpaint");
    cv::namedWindow("gradientX");
    cv::namedWindow("gradientY");

    initializeMats();
    calculateGradients();
    bool stay=true;
    while(stay){

        computeFillFront();
        computeConfidence();
        computeData();
        computeTarget();
        computeBestPatch();
        updateMats();
        stay=checkEnd();

        cv::imshow("updatedMask",updatedMask);
        cv::imshow("inpaint",workImage);
        cv::imshow("gradientX",gradientX);
        cv::imshow("gradientY",gradientY);
        cv::waitKey(2);
    }
    result=workImage.clone();


    cv::namedWindow("confidence");
    cv::imshow("confidence",confidence);
}

void Inpainter::calculateGradients(){
    cv::Mat srcGray;
    cv::cvtColor(workImage,srcGray,CV_BGR2GRAY);

    cv::Scharr(srcGray,gradientX,CV_16S,1,0);
    cv::convertScaleAbs(gradientX,gradientX);
    gradientX.convertTo(gradientX,CV_32F);


    cv::Scharr(srcGray,gradientY,CV_16S,0,1);
    cv::convertScaleAbs(gradientY,gradientY);
    gradientY.convertTo(gradientY,CV_32F);






    for(int x=0;x<sourceRegion.cols;x++){
        for(int y=0;y<sourceRegion.rows;y++){

            if(sourceRegion.at<uchar>(y,x)==0){
                gradientX.at<float>(y,x)=0;
                gradientY.at<float>(y,x)=0;
            }/*else
            {
                if(gradientX.at<float>(y,x)<255)
                    gradientX.at<float>(y,x)=0;
                if(gradientY.at<float>(y,x)<255)
                    gradientY.at<float>(y,x)=0;
            }*/

        }
    }
    gradientX/=255;
    gradientY/=255;
}

void Inpainter::initializeMats(){
    cv::threshold(this->mask,this->confidence,10,255,CV_THRESH_BINARY);
    cv::threshold(confidence,confidence,2,1,CV_THRESH_BINARY_INV);
    confidence.convertTo(confidence,CV_32F);

    this->sourceRegion=confidence.clone();
    this->sourceRegion.convertTo(sourceRegion,CV_8U);
    this->originalSourceRegion=sourceRegion.clone();

    cv::threshold(mask,this->targetRegion,10,255,CV_THRESH_BINARY);
    cv::threshold(targetRegion,targetRegion,2,1,CV_THRESH_BINARY);
    targetRegion.convertTo(targetRegion,CV_8U);
    data=cv::Mat(inputImage.rows,inputImage.cols,CV_32F,cv::Scalar::all(0));


    LAPLACIAN_KERNEL=cv::Mat::ones(3,3,CV_32F);
    LAPLACIAN_KERNEL.at<float>(1,1)=-8;
    NORMAL_KERNELX=cv::Mat::zeros(3,3,CV_32F);
    NORMAL_KERNELX.at<float>(1,0)=-1;
    NORMAL_KERNELX.at<float>(1,2)=1;
    cv::transpose(NORMAL_KERNELX,NORMAL_KERNELY);


}
void Inpainter::computeFillFront(){


    cv::Mat sourceGradientX,sourceGradientY,boundryMat;
    cv::filter2D(targetRegion,boundryMat,CV_32F,LAPLACIAN_KERNEL);
    cv::filter2D(sourceRegion,sourceGradientX,CV_32F,NORMAL_KERNELX);
    cv::filter2D(sourceRegion,sourceGradientY,CV_32F,NORMAL_KERNELY);
    fillFront.clear();
    normals.clear();
    for(int x=0;x<boundryMat.cols;x++){
        for(int y=0;y<boundryMat.rows;y++){

            if(boundryMat.at<float>(y,x)>0){
                fillFront.push_back(cv::Point2i(x,y));

                float dx=sourceGradientX.at<float>(y,x);
                float dy=sourceGradientY.at<float>(y,x);
                cv::Point2f normal(dy,-dx);
                float tempF=std::sqrt((normal.x*normal.x)+(normal.y*normal.y));
                if(tempF!=0){

                normal.x=normal.x/tempF;
                normal.y=normal.y/tempF;

                }
                normals.push_back(normal);

            }
        }
    }


}

void Inpainter::computeConfidence(){
    cv::Point2i a,b;
    for(int i=0;i<fillFront.size();i++){
        cv::Point2i currentPoint=fillFront.at(i);
        getPatch(currentPoint,a,b);
        float total=0;
        for(int x=a.x;x<=b.x;x++){
            for(int y=a.y;y<=b.y;y++){
                if(targetRegion.at<uchar>(y,x)==0){
                    total+=confidence.at<float>(y,x);
                }
            }
        }
        confidence.at<float>(currentPoint.y,currentPoint.x)=total/((b.x-a.x+1)*(b.y-a.y+1));
    }
}

void Inpainter::computeData(){

    for(int i=0;i<fillFront.size();i++){
        cv::Point2i currentPoint=fillFront.at(i);
        cv::Point2i currentNormal=normals.at(i);
        data.at<float>(currentPoint.y,currentPoint.x)=std::fabs(gradientX.at<float>(currentPoint.y,currentPoint.x)*currentNormal.x+gradientY.at<float>(currentPoint.y,currentPoint.x)*currentNormal.y)+.001;
    }
}

void Inpainter::computeTarget(){

    targetIndex=0;
    float maxPriority=0;
    float priority=0;
    cv::Point2i currentPoint;
    for(int i=0;i<fillFront.size();i++){
        currentPoint=fillFront.at(i);
        priority=data.at<float>(currentPoint.y,currentPoint.x)*confidence.at<float>(currentPoint.y,currentPoint.x);
        if(priority>maxPriority){
            maxPriority=priority;
            targetIndex=i;
        }
    }

}

void Inpainter::computeBestPatch(){
    double minError=9999999999999999,bestPatchVarience=9999999999999999;
    cv::Point2i a,b;
    cv::Point2i currentPoint=fillFront.at(targetIndex);
    cv::Vec3b sourcePixel,targetPixel;
    double meanR,meanG,meanB;
    double difference,patchError;
    bool skipPatch;
    getPatch(currentPoint,a,b);

    int width=b.x-a.x+1;
    int height=b.y-a.y+1;
    for(int x=0;x<=workImage.cols-width;x++){
        for(int y=0;y<=workImage.rows-height;y++){
            patchError=0;
            meanR=0;meanG=0;meanB=0;
            skipPatch=false;

            for(int x2=0;x2<width;x2++){
                for(int y2=0;y2<height;y2++){
                    if(originalSourceRegion.at<uchar>(y+y2,x+x2)==0){
                        skipPatch=true;
                        break;
                     }

                    if(sourceRegion.at<uchar>(a.y+y2,a.x+x2)==0)
                        continue;

                    sourcePixel=workImage.at<cv::Vec3b>(y+y2,x+x2);
                    targetPixel=workImage.at<cv::Vec3b>(a.y+y2,a.x+x2);

                    for(int i=0;i<3;i++){
                        difference=sourcePixel[i]-targetPixel[i];
                        patchError+=difference*difference;
                    }
                    meanB+=sourcePixel[0];meanG+=sourcePixel[1];meanR+=sourcePixel[2];


                }
                if(skipPatch)
                    break;
            }

            if(skipPatch)
                continue;
            if(patchError<minError){
                minError=patchError;
                bestMatchUpperLeft=cv::Point2i(x,y);
                bestMatchLowerRight=cv::Point2i(x+width-1,y+height-1);

                double patchVarience=0;
                for(int x2=0;x2<width;x2++){
                    for(int y2=0;y2<height;y2++){
                        if(sourceRegion.at<uchar>(a.y+y2,a.x+x2)==0){
                            sourcePixel=workImage.at<cv::Vec3b>(y+y2,x+x2);
                            difference=sourcePixel[0]-meanB;
                            patchVarience+=difference*difference;
                            difference=sourcePixel[1]-meanG;
                            patchVarience+=difference*difference;
                            difference=sourcePixel[2]-meanR;
                            patchVarience+=difference*difference;
                        }

                    }
                }
                bestPatchVarience=patchVarience;

            }else if(patchError==minError){
                double patchVarience=0;
                for(int x2=0;x2<width;x2++){
                    for(int y2=0;y2<height;y2++){
                        if(sourceRegion.at<uchar>(a.y+y2,a.x+x2)==0){
                            sourcePixel=workImage.at<cv::Vec3b>(y+y2,x+x2);
                            difference=sourcePixel[0]-meanB;
                            patchVarience+=difference*difference;
                            difference=sourcePixel[1]-meanG;
                            patchVarience+=difference*difference;
                            difference=sourcePixel[2]-meanR;
                            patchVarience+=difference*difference;
                        }

                    }
                }
                if(patchVarience<bestPatchVarience){
                    minError=patchError;
                    bestMatchUpperLeft=cv::Point2i(x,y);
                    bestMatchLowerRight=cv::Point2i(x+width-1,y+height-1);
                    bestPatchVarience=patchVarience;
                }
            }
    }
    }


}



void Inpainter::updateMats(){
    cv::Point2i targetPoint=fillFront.at(targetIndex);
    cv::Point2i a,b;
    getPatch(targetPoint,a,b);
    int width=b.x-a.x+1;
    int height=b.y-a.y+1;

    for(int x=0;x<width;x++){
        for(int y=0;y<height;y++){
            if(sourceRegion.at<uchar>(a.y+y,a.x+x)==0){

                workImage.at<cv::Vec3b>(a.y+y,a.x+x)=workImage.at<cv::Vec3b>(bestMatchUpperLeft.y+y,bestMatchUpperLeft.x+x);
                gradientX.at<float>(a.y+y,a.x+x)=gradientX.at<float>(bestMatchUpperLeft.y+y,bestMatchUpperLeft.x+x);
                gradientY.at<float>(a.y+y,a.x+x)=gradientY.at<float>(bestMatchUpperLeft.y+y,bestMatchUpperLeft.x+x);
                confidence.at<float>(a.y+y,a.x+x)=confidence.at<float>(targetPoint.y,targetPoint.x);
                sourceRegion.at<uchar>(a.y+y,a.x+x)=1;
                targetRegion.at<uchar>(a.y+y,a.x+x)=0;
                updatedMask.at<uchar>(a.y+y,a.x+x)=0;
            }
        }
    }


}

bool Inpainter::checkEnd(){
    for(int x=0;x<sourceRegion.cols;x++){
        for(int y=0;y<sourceRegion.rows;y++){
            if(sourceRegion.at<uchar>(y,x)==0){
                return true;
               }
        }

    }
    return false;
}
void Inpainter::getPatch(cv::Point2i &centerPixel, cv::Point2i &upperLeft, cv::Point2i &lowerRight){
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
