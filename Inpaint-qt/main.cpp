#include "inpainter.h"

cv::Mat image,originalImage,inpaintMask;
cv::Point prevPt(-1,-1);
int thickness=5;
static void onMouse( int event, int x, int y, int flags, void* )
{
    if(event == cv::EVENT_LBUTTONUP||!(flags & cv::EVENT_FLAG_LBUTTON) )
        prevPt = cv::Point(-1,-1);
    else if( event == cv::EVENT_LBUTTONDOWN )
        prevPt = cv::Point(x,y);
    else if( event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON) )
    {
        cv::Point pt(x,y);
        if( prevPt.x < 0 )
            prevPt = pt;
        cv::line( inpaintMask, prevPt, pt, cv::Scalar::all(255), thickness, 8, 0 );
        cv::line( image, prevPt, pt, cv::Scalar::all(255), thickness, 8, 0 );
        prevPt = pt;
        cv::imshow("image", image);
    }
}




int main(int argc, char *argv[])
{

    //we expect two arguments.
    //the first is the image path.
    //the second is the mask path.

    //in case only image path is speciifed, we use manual marking of mask over the image
    //in case image name is also not specified , we use default image default.jpg

    char* imageName = argc >= 2 ? argv[1] : (char*)"default.jpg";

    originalImage=cv::imread(imageName,CV_LOAD_IMAGE_COLOR);

    if(!originalImage.data){
        std::cout<<std::endl<<"Error unable to open input image"<<std::endl;
        return 0;
    }

    image=originalImage.clone();



    bool maskSpecified=false;
    char* maskName;
    if(argc >= 3){
       maskName=argv[2];
       maskSpecified=true;
    }

    if(maskSpecified){
        inpaintMask=cv::imread(maskName,CV_LOAD_IMAGE_GRAYSCALE);
        Inpainter i(originalImage,inpaintMask,3);
        if(i.checkValidInputs()==i.CHECK_VALID){
            i.inpaint();
            cv::imwrite("result.jpg",i.result);
            cv::namedWindow("result");
            cv::imshow("result",i.result);
            cv::waitKey();
        }else{
            std::cout<<std::endl<<"Error : invalid parameters"<<std::endl;
        }
    }
    else
    {
        std::cout<<std::endl<<"mask not specified , mark manually on input image"<<std::endl;
        inpaintMask = cv::Mat::zeros(image.size(), CV_8U);
        cv::namedWindow( "image", 1 );
        cv::imshow("image", image);
        cv::setMouseCallback( "image", onMouse, 0 );

        for(;;)
            {
                char c = (char)cv::waitKey();

                if( c == 'e' )
                    break;

                if( c == 'r' )
                {
                    inpaintMask = cv::Scalar::all(0);
                    image=originalImage.clone();
                    cv::imshow("image", image);
                }

                if( c == 'i' || c == ' ' )
                {
                    Inpainter i(originalImage,inpaintMask,3);
                    if(i.checkValidInputs()==i.CHECK_VALID){
                        i.inpaint();
                        cv::imwrite("result.jpg",i.result);
                        inpaintMask = cv::Scalar::all(0);
                        cv::namedWindow("result");
                        cv::imshow("result",i.result);
                    }else{
                        std::cout<<std::endl<<"Error : invalid parameters"<<std::endl;
                    }


                }
                if(c=='s'){
                    thickness++;
                    std::cout<<std::endl<<"Thickness = "<<thickness;
                }
                if(c=='a'){
                    thickness--;
                    std::cout<<std::endl<<"Thickness = "<<thickness;
                }
                if(thickness<3)
                    thickness=3;
                if(thickness>12)
                    thickness=12;
            }

    }





    return 0;
}
