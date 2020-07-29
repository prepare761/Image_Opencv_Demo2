#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat src;

int main(int argc, char** argv)
{
	src = imread("E:/opencv_yao/Img_opencv/airplane.jpg");
	if (src.empty())
	{
		printf("Can not load Image...");
		return -1;
	}
	imshow("input Image",src);

	/*白色背景变成黑色*/
	for (int row=0;row<src.rows;row++)
	{
		for (int col = 0; col < src.cols; col++) {
			if (src.at<Vec3b>(row, col) == Vec3b(255, 255, 255)) {
				src.at<Vec3b>(row, col)[0] = 0;
				src.at<Vec3b>(row, col)[1] = 0;
				src.at<Vec3b>(row, col)[2] = 0;
			}
		}
	}
	imshow("black backgroung", src);
	medianBlur(src,src,3);
	//sharpen(提高对比度)
	Mat kernel = (Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);

	//make it more sharp
	Mat imgLaplance;
	Mat sharpenImg = src;
	//拉普拉斯算子实现边缘提取
	filter2D(src, imgLaplance, CV_32F, kernel, Point(-1, -1), 0, BORDER_DEFAULT);//拉普拉斯有浮点数计算，位数要提高到32
	src.convertTo(sharpenImg, CV_32F);

	//原图减边缘（白色）实现边缘增强
	Mat resultImg = sharpenImg - imgLaplance;

	resultImg.convertTo(resultImg,CV_8UC3);
	imgLaplance.convertTo(imgLaplance, CV_8UC3);
	/*imshow(" Image", imgLaplance);*/
	imshow("sharpen Image", resultImg);

	//转换成二值图
	Mat binary;
	cvtColor(resultImg, resultImg, CV_BGR2GRAY);
	threshold(resultImg, binary,150,255,THRESH_BINARY|THRESH_OTSU);
	imshow("binary image",binary);
//
	//距离变换
	Mat distImg;
	distanceTransform(binary,distImg,CV_DIST_L1,CV_DIST_MASK_3);//寻找每一个点距离最近零点像素的距离信息
	normalize(distImg, distImg, 0, 1, NORM_MINMAX);
	imshow("dist image",distImg);
//
	//二值化
	threshold(distImg, distImg, 0.6, 1, THRESH_BINARY);
	imshow("dist binary image", distImg);
//
	//腐蚀(使得连在一起的部分分开)
	Mat k1 = Mat::ones(3, 3, CV_8UC1);
	erode(distImg, distImg, k1);
	imshow("分开", distImg);
//
	//标记
	Mat dist_8u;
	distImg.convertTo(dist_8u,CV_8UC1);
	vector<vector<Point>> contours;
	findContours(dist_8u, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
//
	//创建标记
	Mat marker = Mat::zeros(src.size(),CV_32SC1);

	//画标记
	for (size_t i = 0; i < contours.size(); i++)
	{
		//i从0取到944
		drawContours(marker,contours,static_cast<int>(i),Scalar(static_cast<int>(i)+1),1);
	}

	circle(marker, Point(5, 5), 3, Scalar(255, 255, 255), -1);
	imshow("marker",marker*1000);
//
	//分水岭变换
	watershed(src,marker);//根据距离变换的标记，在原图上分离
	Mat water = Mat::zeros(marker.size(),CV_8UC1);
	marker.convertTo(water,CV_8UC1);
	bitwise_not(water, water,Mat());//取反操作
	//imshow("源 image", src);
	imshow("watershed Image", water);

	//// generate random color
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++) {
		int r = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int b = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	//// fill with color and display final result
	Mat dst = Mat::zeros(marker.size(), CV_8UC3);
	for (int row = 0; row < marker.rows; row++) {
		for (int col = 0; col < marker.cols; col++) {
			int index = marker.at<int>(row, col);
			if (index > 0 && index <= static_cast<int>(contours.size())) {
				dst.at<Vec3b>(row, col) = colors[index];
			}
			else {
				dst.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
			}
		}
	}
	imshow("Final Result", dst);
	waitKey(0);
	return 0;
}

//#include <iostream>
//#include <opencv2\opencv.hpp>
// 
//using namespace std;
//using namespace cv;
// 
//Mat srcImage, srcImage_, maskImage;
//Mat maskWaterShed;  // watershed()函数的参数
//Point clickPoint;	// 鼠标点下去的位置
// 
//void on_Mouse(int event, int x, int y, int flags, void*);
//void helpText();
// 
//int main(int argc, char** argv)
//{
//	/* 操作提示 */
//	helpText();
// 
//	srcImage = imread("E:/opencv_yao/Img_opencv/fruits.jpg");
//	srcImage_ = srcImage.clone();  // 程序中srcImage会被改变，所以这里做备份
//	maskImage = Mat(srcImage.size(), CV_8UC1);  // 掩模，在上面做标记，然后传给findContours
//	maskImage = Scalar::all(0);
// 
//	int areaCount = 1;  // 计数，在按【0】时绘制每个区域
// 
//	imshow("在图像中做标记", srcImage);
// 
//	setMouseCallback("在图像中做标记", on_Mouse, 0);
// 
//	while (true)
//	{
//		int c = waitKey(0);
// 
//		if ((char)c == 27)	// 按【ESC】键退出
//			break;
// 
//		if ((char)c == '2')  // 按【2】恢复原图
//		{
//			maskImage = Scalar::all(0);
//			srcImage = srcImage_.clone();
//			imshow("在图像中做标记", srcImage);
//		}
// 
//		if ((char)c == '1')  // 按【1】处理图片
//		{
//			vector<vector<Point>> contours;
//			vector<Vec4i> hierarchy;
// 
//			findContours(maskImage, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
// 
//			if (contours.size() == 0)  // 如果没有做标记，即没有轮廓，则退出该if语句
//				break;
//			cout << contours.size() << "个轮廓" << endl;
// 
//			maskWaterShed = Mat(maskImage.size(), CV_32S);
//			maskWaterShed = Scalar::all(0);
// 
//			/* 在maskWaterShed上绘制轮廓 */
//			for (int index = 0; index < contours.size(); index++)
//				drawContours(maskWaterShed, contours, index, Scalar::all(index + 1), -1, 8, hierarchy, INT_MAX);
//			
//			/* 如果imshow这个maskWaterShed，我们会发现它是一片黑，原因是在上面我们只给它赋了1,2,3这样的值，通过代码80行的处理我们才能清楚的看出结果 */
//			watershed(srcImage_, maskWaterShed);  // 注释一
// 
//			vector<Vec3b> colorTab;  // 随机生成几种颜色
//			for (int i = 0; i < contours.size(); i++)
//			{
//				int b = theRNG().uniform(0, 255);
//				int g = theRNG().uniform(0, 255);
//				int r = theRNG().uniform(0, 255);
// 
//				colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
//			}
// 
//			Mat resImage = Mat(srcImage.size(), CV_8UC3);  // 声明一个最后要显示的图像
//			for (int i = 0; i < maskImage.rows; i++)
//			{
//				for (int j = 0; j < maskImage.cols; j++)
//				{	// 根据经过watershed处理过的maskWaterShed来绘制每个区域的颜色
//					int index = maskWaterShed.at<int>(i, j);  // 这里的maskWaterShed是经过watershed处理的
//					if (index == -1)  // 区域间的值被置为-1（边界）
//						resImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
//					else if (index <= 0 || index > contours.size())  // 没有标记清楚的区域被置为0
//						resImage.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
//					else  // 其他每个区域的值保持不变：1，2，...，contours.size()
//						resImage.at<Vec3b>(i, j) = colorTab[index - 1];  // 然后把这些区域绘制成不同颜色
//				}
//			}
//			imshow("resImage", resImage);
//			addWeighted(resImage, 0.3, srcImage_, 0.7, 0, resImage);
//			imshow("分水岭结果", resImage);
//		}
// 
//		if ((char)c == '0')  // 多次点按【0】依次显示每个被分割的区域，需要先按【1】处理图像
//		{
//			Mat resImage = Mat(srcImage.size(), CV_8UC3);  // 声明一个最后要显示的图像
//			for (int i = 0; i < maskImage.rows; i++)
//			{
//				for (int j = 0; j < maskImage.cols; j++)
//				{
//					int index = maskWaterShed.at<int>(i, j);
//					if (index == areaCount)
//						resImage.at<Vec3b>(i, j) = srcImage_.at<Vec3b>(i, j);
//					else
//						resImage.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
//				}
//			}
//			imshow("分水岭结果", resImage);
//			areaCount++;
//			if (areaCount == 4)
//				areaCount = 1;
//		}
//	}
// 
//	return 0;
//}
// 
//void on_Mouse(int event, int x, int y, int flags, void*)
//{
//	// 如果鼠标不在窗口中则返回
//	if (x < 0 || x >= srcImage.cols || y < 0 || y >= srcImage.rows)
//		return;
// 
//	// 如果鼠标左键被按下，获取鼠标当前位置；当鼠标左键按下并且移动时，绘制白线；
//	if (event == EVENT_LBUTTONDOWN)
//	{
//		clickPoint = Point(x, y);
//	}		
//	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
//	{
//		Point point(x, y);
//		line(maskImage, clickPoint, point, Scalar::all(255), 5, 8, 0);
//		line(srcImage, clickPoint, point, Scalar::all(255), 5, 8, 0);
//		clickPoint = point;
//		imshow("在图像中做标记", srcImage);
//	}
//}
// 
//void helpText()
//{
//	cout << "先用鼠标在图片窗口中标记出大致的区域" << endl;
//	cout << "如果想把图片分割为N个区域，就要做N个标记" << endl;
//	cout << "键盘按键【1】	- 运行的分水岭分割算法" << endl;
//	cout << "键盘按键【2】	- 恢复原始图片" << endl;
//	cout << "键盘按键【0】	- 依次分割每个区域（必须先按【1】）" << endl;
//	cout << "键盘按键【ESC】	- 退出程序" << endl << endl;
//}
 
/* 注释一：watershed(srcImage_, maskWaterShed);
 * 注意它的两个参数
 * srcImage_是没做任何修改的原图，CV_8UC3类型
 * maskWaterShed声明为CV_32S类型（32位单通道），且全部元素为0
 *		然后作为drawContours的第一个参数传入，在上面绘制轮廓
 *		最后作为watershed的参数
 * 另外，watershed的第二个参数maskWaterShed是InputOutputArray类型
 *		即作为输入，也作为输出保存函数调用的结果
 */