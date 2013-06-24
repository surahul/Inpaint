

#include <inpainter.h>

int main(int argc, char *argv[])
{


    cv::Mat image=cv::imread("image7.jpg",CV_LOAD_IMAGE_COLOR);
    cv::Mat mask=cv::imread("mas7.jpg",CV_LOAD_IMAGE_GRAYSCALE);
    inpainter i;
    cv::Mat result;
    i.inpaint(image,result,mask,false);
    cv::namedWindow("a");
    cv::imshow("a",i.workImage);
    cv::imwrite("result.jpg",i.workImage);
    cv::waitKey();
    return 0;
}
