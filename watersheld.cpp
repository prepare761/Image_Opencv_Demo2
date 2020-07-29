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

	/*��ɫ������ɺ�ɫ*/
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
	//sharpen(��߶Աȶ�)
	Mat kernel = (Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);

	//make it more sharp
	Mat imgLaplance;
	Mat sharpenImg = src;
	//������˹����ʵ�ֱ�Ե��ȡ
	filter2D(src, imgLaplance, CV_32F, kernel, Point(-1, -1), 0, BORDER_DEFAULT);//������˹�и��������㣬λ��Ҫ��ߵ�32
	src.convertTo(sharpenImg, CV_32F);

	//ԭͼ����Ե����ɫ��ʵ�ֱ�Ե��ǿ
	Mat resultImg = sharpenImg - imgLaplance;

	resultImg.convertTo(resultImg,CV_8UC3);
	imgLaplance.convertTo(imgLaplance, CV_8UC3);
	/*imshow(" Image", imgLaplance);*/
	imshow("sharpen Image", resultImg);

	//ת���ɶ�ֵͼ
	Mat binary;
	cvtColor(resultImg, resultImg, CV_BGR2GRAY);
	threshold(resultImg, binary,150,255,THRESH_BINARY|THRESH_OTSU);
	imshow("binary image",binary);
//
	//����任
	Mat distImg;
	distanceTransform(binary,distImg,CV_DIST_L1,CV_DIST_MASK_3);//Ѱ��ÿһ����������������صľ�����Ϣ
	normalize(distImg, distImg, 0, 1, NORM_MINMAX);
	imshow("dist image",distImg);
//
	//��ֵ��
	threshold(distImg, distImg, 0.6, 1, THRESH_BINARY);
	imshow("dist binary image", distImg);
//
	//��ʴ(ʹ������һ��Ĳ��ַֿ�)
	Mat k1 = Mat::ones(3, 3, CV_8UC1);
	erode(distImg, distImg, k1);
	imshow("�ֿ�", distImg);
//
	//���
	Mat dist_8u;
	distImg.convertTo(dist_8u,CV_8UC1);
	vector<vector<Point>> contours;
	findContours(dist_8u, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
//
	//�������
	Mat marker = Mat::zeros(src.size(),CV_32SC1);

	//�����
	for (size_t i = 0; i < contours.size(); i++)
	{
		//i��0ȡ��944
		drawContours(marker,contours,static_cast<int>(i),Scalar(static_cast<int>(i)+1),1);
	}

	circle(marker, Point(5, 5), 3, Scalar(255, 255, 255), -1);
	imshow("marker",marker*1000);
//
	//��ˮ��任
	watershed(src,marker);//���ݾ���任�ı�ǣ���ԭͼ�Ϸ���
	Mat water = Mat::zeros(marker.size(),CV_8UC1);
	marker.convertTo(water,CV_8UC1);
	bitwise_not(water, water,Mat());//ȡ������
	//imshow("Դ image", src);
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
//Mat maskWaterShed;  // watershed()�����Ĳ���
//Point clickPoint;	// ������ȥ��λ��
// 
//void on_Mouse(int event, int x, int y, int flags, void*);
//void helpText();
// 
//int main(int argc, char** argv)
//{
//	/* ������ʾ */
//	helpText();
// 
//	srcImage = imread("E:/opencv_yao/Img_opencv/fruits.jpg");
//	srcImage_ = srcImage.clone();  // ������srcImage�ᱻ�ı䣬��������������
//	maskImage = Mat(srcImage.size(), CV_8UC1);  // ��ģ������������ǣ�Ȼ�󴫸�findContours
//	maskImage = Scalar::all(0);
// 
//	int areaCount = 1;  // �������ڰ���0��ʱ����ÿ������
// 
//	imshow("��ͼ���������", srcImage);
// 
//	setMouseCallback("��ͼ���������", on_Mouse, 0);
// 
//	while (true)
//	{
//		int c = waitKey(0);
// 
//		if ((char)c == 27)	// ����ESC�����˳�
//			break;
// 
//		if ((char)c == '2')  // ����2���ָ�ԭͼ
//		{
//			maskImage = Scalar::all(0);
//			srcImage = srcImage_.clone();
//			imshow("��ͼ���������", srcImage);
//		}
// 
//		if ((char)c == '1')  // ����1������ͼƬ
//		{
//			vector<vector<Point>> contours;
//			vector<Vec4i> hierarchy;
// 
//			findContours(maskImage, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
// 
//			if (contours.size() == 0)  // ���û������ǣ���û�����������˳���if���
//				break;
//			cout << contours.size() << "������" << endl;
// 
//			maskWaterShed = Mat(maskImage.size(), CV_32S);
//			maskWaterShed = Scalar::all(0);
// 
//			/* ��maskWaterShed�ϻ������� */
//			for (int index = 0; index < contours.size(); index++)
//				drawContours(maskWaterShed, contours, index, Scalar::all(index + 1), -1, 8, hierarchy, INT_MAX);
//			
//			/* ���imshow���maskWaterShed�����ǻᷢ������һƬ�ڣ�ԭ��������������ֻ��������1,2,3������ֵ��ͨ������80�еĴ������ǲ�������Ŀ������ */
//			watershed(srcImage_, maskWaterShed);  // ע��һ
// 
//			vector<Vec3b> colorTab;  // ������ɼ�����ɫ
//			for (int i = 0; i < contours.size(); i++)
//			{
//				int b = theRNG().uniform(0, 255);
//				int g = theRNG().uniform(0, 255);
//				int r = theRNG().uniform(0, 255);
// 
//				colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
//			}
// 
//			Mat resImage = Mat(srcImage.size(), CV_8UC3);  // ����һ�����Ҫ��ʾ��ͼ��
//			for (int i = 0; i < maskImage.rows; i++)
//			{
//				for (int j = 0; j < maskImage.cols; j++)
//				{	// ���ݾ���watershed�������maskWaterShed������ÿ���������ɫ
//					int index = maskWaterShed.at<int>(i, j);  // �����maskWaterShed�Ǿ���watershed�����
//					if (index == -1)  // ������ֵ����Ϊ-1���߽磩
//						resImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
//					else if (index <= 0 || index > contours.size())  // û�б�������������Ϊ0
//						resImage.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
//					else  // ����ÿ�������ֵ���ֲ��䣺1��2��...��contours.size()
//						resImage.at<Vec3b>(i, j) = colorTab[index - 1];  // Ȼ�����Щ������Ƴɲ�ͬ��ɫ
//				}
//			}
//			imshow("resImage", resImage);
//			addWeighted(resImage, 0.3, srcImage_, 0.7, 0, resImage);
//			imshow("��ˮ����", resImage);
//		}
// 
//		if ((char)c == '0')  // ��ε㰴��0��������ʾÿ�����ָ��������Ҫ�Ȱ���1������ͼ��
//		{
//			Mat resImage = Mat(srcImage.size(), CV_8UC3);  // ����һ�����Ҫ��ʾ��ͼ��
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
//			imshow("��ˮ����", resImage);
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
//	// �����겻�ڴ������򷵻�
//	if (x < 0 || x >= srcImage.cols || y < 0 || y >= srcImage.rows)
//		return;
// 
//	// ��������������£���ȡ��굱ǰλ�ã������������²����ƶ�ʱ�����ư��ߣ�
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
//		imshow("��ͼ���������", srcImage);
//	}
//}
// 
//void helpText()
//{
//	cout << "���������ͼƬ�����б�ǳ����µ�����" << endl;
//	cout << "������ͼƬ�ָ�ΪN�����򣬾�Ҫ��N�����" << endl;
//	cout << "���̰�����1��	- ���еķ�ˮ��ָ��㷨" << endl;
//	cout << "���̰�����2��	- �ָ�ԭʼͼƬ" << endl;
//	cout << "���̰�����0��	- ���ηָ�ÿ�����򣨱����Ȱ���1����" << endl;
//	cout << "���̰�����ESC��	- �˳�����" << endl << endl;
//}
 
/* ע��һ��watershed(srcImage_, maskWaterShed);
 * ע��������������
 * srcImage_��û���κ��޸ĵ�ԭͼ��CV_8UC3����
 * maskWaterShed����ΪCV_32S���ͣ�32λ��ͨ��������ȫ��Ԫ��Ϊ0
 *		Ȼ����ΪdrawContours�ĵ�һ���������룬�������������
 *		�����Ϊwatershed�Ĳ���
 * ���⣬watershed�ĵڶ�������maskWaterShed��InputOutputArray����
 *		����Ϊ���룬Ҳ��Ϊ������溯�����õĽ��
 */