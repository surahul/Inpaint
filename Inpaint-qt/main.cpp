

#include "inpainter.h"

int main(int argc, char *argv[])
{
    // We expect two arguments.
    // The first is the image path.
    // The second is the mask path.
    assert(argc == 3);

    const std::string imagePath(argv[1]);
    const std::string maskPath(argv[2]);

    cv::Mat image=cv::imread(imagePath,CV_LOAD_IMAGE_COLOR);
    cv::Mat mask=cv::imread(maskPath,CV_LOAD_IMAGE_GRAYSCALE);
    inpainter i;
    cv::Mat result;
    i.inpaint(image,result,mask,false);
    cv::namedWindow("a");
    cv::imshow("a",i.workImage);
    cv::imwrite("result.jpg",i.workImage);
    cv::waitKey();
    return 0;
}
