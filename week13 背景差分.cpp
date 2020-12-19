#include<iostream>
#include<opencv2/opencv.hpp>


using namespace cv;
using namespace std;

int main() {
	VideoCapture cap;
	cap.open(0);
	int cnt = 0;
	Mat frame;	//当前帧
	Mat back;	//背景
	Mat sub;//差分
	Mat gray_frame;
	while (1) {
		if (cnt == 0) {
			cap >> frame;
			cvtColor(frame, gray_frame, COLOR_BGR2GRAY);	//灰度图
			gray_frame.copyTo(back);	//第一帧为背景
		}
		
		else{
			cap >> frame;
			cvtColor(frame, gray_frame, COLOR_BGR2GRAY); //当前帧(灰度图)
			gray_frame.copyTo(sub);	//格式确定
			for (int i = 0; i < gray_frame.rows; i++) {
				for (int j = 0; j < gray_frame.cols; j++) {
					sub.at<uchar>(i, j) = gray_frame.at<uchar>(i, j) - back.at<uchar>(i, j);
				}
			}
			namedWindow("差分图像");
			imshow("差分图像", sub);
		}
		cnt++;
		waitKey(30);
	
	}
	//----------------------------------------------------高斯建模----------------------------------------------------------------

	int n = 50;		//取前50帧画面
	Mat var[50];
	Mat mean_sum;
	
	Mat mean;
	
	
	cap >> frame;
	cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
	
	mean_sum.create(480, 640 ,CV_16U);

	for (int i = 0; i < mean_sum.rows; i++) {			//初始化mean_sum Mat 用来累加
		for (int j = 0; j < mean_sum.cols; j++) {
			mean_sum.at<ushort>(i, j) = 0;
		}
	}


	//等待一段时间，取50帧画面
	int trans=0;
	while (cnt<n) {			//把前50帧Mat都累加到mean_sum中

		cap >> frame;
		cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
		imshow("",gray_frame);

		for (int i = 0; i < gray_frame.rows; i++) {
			for (int j = 0; j < gray_frame.cols; j++) {
				trans= gray_frame.at<uchar>(i, j);		//sumMat
				mean_sum.at<ushort>(i, j) += trans;
			}
		}
		gray_frame.copyTo(var[cnt]);			//储存当前帧图像

		cnt++;
		waitKey(30);
		
	}
	mean.create(mean_sum.rows,mean_sum.cols,CV_16UC1);

	for (int i = 0; i < gray_frame.rows; i++) {
		for (int j = 0; j < gray_frame.cols; j++) {
			mean.at<ushort>(i, j) = round(mean_sum.at<ushort>(i, j) / n);		//从mean_sum中得到均值Mat
		}
	}

	int sum = 0;
	Mat varMat;		//方差Mat
	varMat.create(480, 640, CV_8U);
	for (int i = 0; i < varMat.rows; i++) {
		for (int j = 0; j < varMat.cols; j++) {
			for (int x = 0; x < n; x++) {
				sum += pow((var[x].at<uchar>(i, j) - mean.at<ushort>(i, j)), 2);	//方差计算公式
			}
			varMat.at<uchar>(i, j) = sum / n;
			varMat.at<uchar>(i, j) = sqrt(varMat.at<uchar>(i, j));		//开根号
		}

	}




	Mat frame_out;
	Mat element;		//腐蚀算子
	element.create(5,5,CV_8U);
	while (1) {
		cap >> frame;
		cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
		for (int i = 0; i < gray_frame.rows; i++) {
			for (int j = 0; j < gray_frame.cols; j++) {
				//判断条件，即不在3sigma区域内的判定为移动的像素，并设置为白色
				if (((gray_frame.at<uchar>(i, j)) < ((mean.at<ushort>(i, j)) - (1.5 * varMat.at<uchar>(i, j)))) || ((gray_frame.at<uchar>(i, j)) > ((mean.at<ushort>(i, j)) + (1.5 * varMat.at<uchar>(i, j))))) {
					gray_frame.at<uchar>(i, j) = 255;
				}
				//否则设置为黑色
				else {
					gray_frame.at<uchar>(i, j) = 0;
				}

			}

		}
		erode(gray_frame, frame_out, element);	//为消除噪声，做适当腐蚀

		imshow("", frame_out);  //最终画面



		waitKey(30);
	}

}