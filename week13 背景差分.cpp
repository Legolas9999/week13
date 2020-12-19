#include<iostream>
#include<opencv2/opencv.hpp>


using namespace cv;
using namespace std;

int main() {
	VideoCapture cap;
	cap.open(0);
	int cnt = 0;
	Mat frame;	//��ǰ֡
	Mat back;	//����
	Mat sub;//���
	Mat gray_frame;
	while (1) {
		if (cnt == 0) {
			cap >> frame;
			cvtColor(frame, gray_frame, COLOR_BGR2GRAY);	//�Ҷ�ͼ
			gray_frame.copyTo(back);	//��һ֡Ϊ����
		}
		
		else{
			cap >> frame;
			cvtColor(frame, gray_frame, COLOR_BGR2GRAY); //��ǰ֡(�Ҷ�ͼ)
			gray_frame.copyTo(sub);	//��ʽȷ��
			for (int i = 0; i < gray_frame.rows; i++) {
				for (int j = 0; j < gray_frame.cols; j++) {
					sub.at<uchar>(i, j) = gray_frame.at<uchar>(i, j) - back.at<uchar>(i, j);
				}
			}
			namedWindow("���ͼ��");
			imshow("���ͼ��", sub);
		}
		cnt++;
		waitKey(30);
	
	}
	//----------------------------------------------------��˹��ģ----------------------------------------------------------------

	int n = 50;		//ȡǰ50֡����
	Mat var[50];
	Mat mean_sum;
	
	Mat mean;
	
	
	cap >> frame;
	cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
	
	mean_sum.create(480, 640 ,CV_16U);

	for (int i = 0; i < mean_sum.rows; i++) {			//��ʼ��mean_sum Mat �����ۼ�
		for (int j = 0; j < mean_sum.cols; j++) {
			mean_sum.at<ushort>(i, j) = 0;
		}
	}


	//�ȴ�һ��ʱ�䣬ȡ50֡����
	int trans=0;
	while (cnt<n) {			//��ǰ50֡Mat���ۼӵ�mean_sum��

		cap >> frame;
		cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
		imshow("",gray_frame);

		for (int i = 0; i < gray_frame.rows; i++) {
			for (int j = 0; j < gray_frame.cols; j++) {
				trans= gray_frame.at<uchar>(i, j);		//sumMat
				mean_sum.at<ushort>(i, j) += trans;
			}
		}
		gray_frame.copyTo(var[cnt]);			//���浱ǰ֡ͼ��

		cnt++;
		waitKey(30);
		
	}
	mean.create(mean_sum.rows,mean_sum.cols,CV_16UC1);

	for (int i = 0; i < gray_frame.rows; i++) {
		for (int j = 0; j < gray_frame.cols; j++) {
			mean.at<ushort>(i, j) = round(mean_sum.at<ushort>(i, j) / n);		//��mean_sum�еõ���ֵMat
		}
	}

	int sum = 0;
	Mat varMat;		//����Mat
	varMat.create(480, 640, CV_8U);
	for (int i = 0; i < varMat.rows; i++) {
		for (int j = 0; j < varMat.cols; j++) {
			for (int x = 0; x < n; x++) {
				sum += pow((var[x].at<uchar>(i, j) - mean.at<ushort>(i, j)), 2);	//������㹫ʽ
			}
			varMat.at<uchar>(i, j) = sum / n;
			varMat.at<uchar>(i, j) = sqrt(varMat.at<uchar>(i, j));		//������
		}

	}




	Mat frame_out;
	Mat element;		//��ʴ����
	element.create(5,5,CV_8U);
	while (1) {
		cap >> frame;
		cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
		for (int i = 0; i < gray_frame.rows; i++) {
			for (int j = 0; j < gray_frame.cols; j++) {
				//�ж�������������3sigma�����ڵ��ж�Ϊ�ƶ������أ�������Ϊ��ɫ
				if (((gray_frame.at<uchar>(i, j)) < ((mean.at<ushort>(i, j)) - (1.5 * varMat.at<uchar>(i, j)))) || ((gray_frame.at<uchar>(i, j)) > ((mean.at<ushort>(i, j)) + (1.5 * varMat.at<uchar>(i, j))))) {
					gray_frame.at<uchar>(i, j) = 255;
				}
				//��������Ϊ��ɫ
				else {
					gray_frame.at<uchar>(i, j) = 0;
				}

			}

		}
		erode(gray_frame, frame_out, element);	//Ϊ�������������ʵ���ʴ

		imshow("", frame_out);  //���ջ���



		waitKey(30);
	}

}