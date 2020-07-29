/*
*ͼ���(Image Moments) ���ξ� ���ľ� ���Ĺ�һ����
        ��ֵͼ����ȡ�Ķ��󣬶��Ǹ����������������Ǹ��������м��ξصļ��㣬��ô���Ǿ͵õ�������������һ�������ף����ף�����
* API������ʹ��-�����cv::moments
        moments(
                InputArray  array,//�������� ����
                bool   binaryImage=false // �Ƿ�Ϊ��ֵͼ��
        )
        contourArea(
                InputArray  contour,//������������ ��ͼ������
                bool   oriented// Ĭ��false�����ؾ���ֵ
        )
        arcLength(
                InputArray  curve,//������������
                bool   closed// �Ƿ��Ƿ������
        )
        ����
		1����ȡͼ���Ե
		2����������
		3������ÿ����������ľ�
		4������ÿ����������ġ����������
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
    GaussianBlur(gray_src, gray_src, Size(3, 3), 0, 0); //Canny�������Ƚ�����
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
 
    //��ÿһ�������������ľأ��õ�ͼ�����ģ�ͬ�����Լ��������������������Щ���Զ�ͼ���еĶ������һЩ����
    vector<Moments> contours_moments(contours.size()); //ÿ��contour��Ҫ��һ��Moments�Ͷ���һ��Moments���飬�ǵø�����ʼ��һ�´�С
    vector<Point2f> ccs(contours.size()); //��ȡ��������λ�ã��������ÿһ��Moments��������λ�ã����ģ�
    for (size_t i = 0; i < contours.size(); i++)
    {
        //��i��Moments�͵�i������λ��
        contours_moments[i] = moments(contours[i]);
        ccs[i] = Point(static_cast<float>(contours_moments[i].m10 / contours_moments[i].m00), static_cast<float>(contours_moments[i].m01 / contours_moments[i].m00));
        //static_cast<float> c++����ĺ�����ǿ��ת��
    }
 
    //������λ�û��Ƴ��� ��contour����������Ƴ���
    Mat drawImg;// = Mat::zeros(src.size(), CV_8UC3);
    src.copyTo(drawImg);
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contours[i].size() < 100) //contourС��һ���̶ȵģ������Ƚ��ٵľͲ�ҪŪ����
        {
            continue;
        }
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        printf("center point x : %.2f y : %.2f\n", ccs[i].x, ccs[i].y);
        printf("contours %d area : %.2f   arc length : %.2f\n", i, contourArea(contours[i]), arcLength(contours[i], true)); //��i������ͻ���
        drawContours(drawImg, contours, i, color, 2, 8, hierachy, 0, Point(0, 0));
        circle(drawImg, ccs[i], 2, color,2, 8);
    }
 
    imshow(output_win, drawImg);
    return;
}