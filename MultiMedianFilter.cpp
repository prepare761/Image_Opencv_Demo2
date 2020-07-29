#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
//多级中值滤波
void YZmedian(Mat& I, Mat& Iout, int n=3, double k=1.6)
{
	cvtColor(I,I,COLOR_BGR2GRAY);//灰度图转换
	Mat temp_m,temp_std;   //定义均值方差矩阵
	meanStdDev(I,temp_m,temp_std);//求均值标准差矩阵
	double Im_m=temp_m.at<double>(0,0);//均值
	double Im_std=temp_std.at<double>(0,0);//标准差


	double t1=Im_m-k*Im_std;//阈值1
	double t2=Im_m+k*Im_std;//阈值2

	Mat I1=I.clone();
	Mat I2=I.clone();

	int width=I.cols;
	int height=I.rows;
	for (int i = 0; i < height; i++)
	{     
		uchar* data1 =I1.ptr<uchar>(i);
		uchar* data2=I2.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			if (data1[j]>=t1)
			{
				data1[j]=1;
			}
			else
			{
				data1[j]=0;
			}
			if (data2[j]>=t2)
			{
				data2[j]=1;
			}
			else
			{
				data2[j]=0;
			}
		}
	}

	Mat I11,I22;
	medianBlur(I1,I11,n);//中值滤波
	medianBlur(I2,I22,n);//中值滤波

	Mat I111,I222;
	bitwise_xor(I1,I11,I111);
	bitwise_xor(I2,I22,I222);//噪声点特别明显的

	Mat I3;
	bitwise_or(I111,I222,I3);//过亮和过暗进行综合

	/*扩展边界方便滤波*/
	Mat I4(Size(I3.rows+(n-1),I3.cols+(n-1)),CV_8U,Scalar(0));
	Mat I5(Size(I3.rows+(n-1),I3.cols+(n-1)),CV_8U,Scalar(0));
	I.copyTo(I4(Range((n-1)/2,I4.rows-(n-1)/2),Range((n-1)/2,I4.cols-(n-1)/2)));
	I3.copyTo(I5(Range((n-1)/2,I4.rows-(n-1)/2),Range((n-1)/2,I4.cols-(n-1)/2)));

	//在噪声处滤波
	for (int i = 0; i < I4.rows; i++)
	{
		uchar* data1=I4.ptr<uchar>(i);
		uchar* data2=I5.ptr<uchar>(i);
		for (int j = 0; j < I4.cols; j++)
		{
			if(data2[j]==1)
			{
				Mat roi=I4(Range(i-(n-1)/2,i+(n-1)/2),Range(j-(n-1)/2,j+(n-1)/2));
				resize(roi,roi,Size(1,n*n));
				Mat index;
				sortIdx(roi,index,SORT_EVERY_COLUMN+SORT_ASCENDING);
				int x=index.at<int>((n*n-1)/2);
				data1[j]=roi.at<uchar>(x);
			}
		}
	}
	Iout=I4(Range((n-1)/2,I4.rows-(n-1)/2),Range((n-1)/2,I4.cols-(n-1)/2)).clone(); 

}
int main()
{  

	Mat I=imread("E:/Detect/Image_Opencv/image/fruits.jpg");//读取原始图片
	Mat Iout;
	Mat Imdian;
	medianBlur(I,Imdian,7);
	YZmedian(I,Iout,7,1.6);
	imshow("原始图像",I);
	imshow("基于阈值分解的多级中值滤波后的图像",Iout);
	imshow("传统中值滤波",Imdian);

	waitKey(0);
	return 0;
}
