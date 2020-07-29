#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

int main(int agrc,char*agrv[])
{
	Mat src =imread("E:/Detect/Image_Opencv/image/666666.png",IMREAD_GRAYSCALE);
	/*cvtColor(src,src,CV_BGR2GRAY);*/
	/*normalize(src,src,0,255,NORM_MINMAX,-1);*/
	Ptr<CLAHE> clahe =createCLAHE(10,Size(7,7));
	Mat dst,dst2;
	clahe->apply(src,dst);
	equalizeHist(src,dst2);
	/*medianBlur(dst,dst,1);*/
	imshow("原图",src);
	imshow("限制",dst);
	imshow("均衡",dst2);
	imwrite("限制.png",dst);
	imwrite("均衡.png",dst2);
	waitKey(0);
	return 0;
}