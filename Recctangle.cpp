/*
*图像矩(Image Moments) 几何矩 中心矩 中心归一化矩
        二值图像提取的对象，对那个对象获得轮廓，对那个轮廓进行几何矩的计算，那么我们就得到轮廓的特征，一般计算二阶，三阶，弧矩
* API介绍与使用-计算矩cv::moments
        moments(
                InputArray  array,//输入数据 轮廓
                bool   binaryImage=false // 是否为二值图像
        )
        contourArea(
                InputArray  contour,//输入轮廓数据 求图像的面积
                bool   oriented// 默认false、返回绝对值
        )
        arcLength(
                InputArray  curve,//输入曲线数据
                bool   closed// 是否是封闭曲线
        )
        步骤
		1：提取图像边缘
		2：发现轮廓
		3：计算每个轮廓对象的矩
		4：计算每个对象的中心、弧长、面积
*/
 
#include <iostream>
#include <opencv2/opencv.hpp>
 
using namespace std;
using namespace cv;
 
Mat src, gray_src;
int threshold_value = 80;
int threshold_max = 255;
RNG rng(12345);

char* output_win = "image moment demo";
void Demo_Moments(int, void*);
 
int main()
{
    src = imread("E:/opencv_yao/Img_opencv/circle111.jpg");
    char *input_win="input image";
	cvtColor(src, gray_src, CV_BGR2GRAY);
    GaussianBlur(gray_src, gray_src, Size(3, 3), 0, 0); //Canny对噪声比较敏感
	namedWindow(input_win,CV_WINDOW_AUTOSIZE);
	namedWindow(output_win,CV_WINDOW_AUTOSIZE);
	imshow(input_win, src);
	
	
    createTrackbar("Threshold Value : ", output_win, &threshold_value, threshold_max, Demo_Moments);
    Demo_Moments(0, 0);
 
    waitKey(0);
    return 0;
}
 
void Demo_Moments(int, void*)
{
    Mat canny_output;
    Canny(gray_src, canny_output, threshold_value, threshold_value * 2, 3, false);
 
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(canny_output, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
 
    //对每一个轮廓计算它的矩，得到图像中心，同样可以计算它的面积跟弧长，这些可以对图像当中的对象进行一些度量
    vector<Moments> contours_moments(contours.size()); //每个contour都要有一个Moments就定义一个Moments数组，记得给它初始化一下大小
    vector<Point2f> ccs(contours.size()); //获取它的中心位置，计算出来每一个Moments都有中心位置（质心）
    for (size_t i = 0; i < contours.size(); i++)
    {
        //第i个Moments和第i个中心位置
        contours_moments[i] = moments(contours[i]);
        ccs[i] = Point(static_cast<float>(contours_moments[i].m10 / contours_moments[i].m00), static_cast<float>(contours_moments[i].m01 / contours_moments[i].m00));
        //static_cast<float> c++里面的函数，强制转型
    }
 
    //把中心位置绘制出来 在contour上面给它绘制出来
    Mat drawImg;// = Mat::zeros(src.size(), CV_8UC3);
    src.copyTo(drawImg);
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contours[i].size() < 100) //contour小于一定程度的，点数比较少的就不要弄出来
        {
            continue;
        }
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        printf("center point x : %.2f y : %.2f\n", ccs[i].x, ccs[i].y);
        printf("contours %d area : %.2f   arc length : %.2f\n", i, contourArea(contours[i]), arcLength(contours[i], true)); //第i个面积和弧长
        drawContours(drawImg, contours, i, color, 2, 8, hierachy, 0, Point(0, 0));
        circle(drawImg, ccs[i], 2, color,2, 8);
    }
 
    imshow(output_win, drawImg);
    return;
}