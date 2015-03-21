#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

#include "VehicleColorRecognition.h"

using namespace std;
using namespace cv;

string hsvRecognition(const Mat& h_plane, const Mat& s_plane, const Mat& v_plane);

string recognizeVehicleColor(const Mat& bgrImg)
{
    CV_Assert(bgrImg.channels() == 3);

    /******************************************************/
    /*  ����ÿ�����ص� B/G/R �е����ֵ����Сֵ�Ĳ�  */
    /******************************************************/
    vector<Mat> bgr_planes;
    cv::split(bgrImg, bgr_planes);    // ����BGRͨ��

    // �����μ���õ�ÿ������ BGR ��ͨ���е����ֵ
    Mat bgrMaxPixels;
    cv::max(bgr_planes[0], bgr_planes[1], bgrMaxPixels);
    cv::max(bgrMaxPixels, bgr_planes[2], bgrMaxPixels);

    // �����μ���õ�ÿ������ BGR ��ͨ���е���Сֵ
    Mat bgrMinPixels;
    cv::min(bgr_planes[0], bgr_planes[1], bgrMinPixels);
    cv::min(bgrMinPixels, bgr_planes[2], bgrMinPixels);

    // ���� BGR ��ͨ�������ֵ����Сֵ�Ĳ�ֵ
    Mat bgrDiff;
    cv::subtract(bgrMaxPixels, bgrMinPixels, bgrDiff);

    // ������Ĳ�ֵͼ�������ǿ����ǿЧ����ʶ��׼ȷ���нϴ�Ӱ��
    // OpenCV û���� Matlab �е� imadjust ���Ӧ�ĺ������������
    cv::equalizeHist(bgrDiff, bgrDiff);    // �������һ
    //cv::normalize( bgrDiff, bgrDiff, 0, 255 );  // ���������    
    cv::max(bgrDiff, bgrMinPixels, bgrDiff);

    // Otsu ��ֵ�ָ�
    Mat bgrMask;
    double otsu_thresh = cv::threshold(bgrDiff, bgrMask, 0, 255, THRESH_BINARY | THRESH_OTSU);
    cv::threshold(bgrDiff, bgrMask, otsu_thresh + 25, 255, THRESH_BINARY);

    //namedWindow( "Car Mask", 1 );
    //imshow( "Car Mask", bgrMask );

    // HSV ��ͨ������
    Mat hsvImg;
    cv::cvtColor(bgrImg, hsvImg, CV_BGR2HSV);
    vector<Mat> hsv_planes;
    cv::split(hsvImg, hsv_planes);    // ����HSVͨ��

    // ���ö�ֵͼ bgrMask �� HSV ��ͨ���еı���������Ϊ 0.
    cv::multiply(hsv_planes[0], bgrMask / 255, hsv_planes[0]);
    cv::multiply(hsv_planes[1], bgrMask / 255, hsv_planes[1]);
    cv::multiply(hsv_planes[2], bgrMask / 255, hsv_planes[2]);

    // ���� HSV ��ͨ��������ɫʶ��
    string color = hsvRecognition(hsv_planes[0], hsv_planes[1], hsv_planes[2]);

    return color;
}


string hsvRecognition(const Mat& h_plane, const Mat& s_plane, const Mat& v_plane)
{
    // �ֱ��������ͨ������ɫֱ��ͼ
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    Mat h_hist, s_hist, v_hist;
    calcHist(&h_plane, 1, 0, Mat(), h_hist, 1, &histSize, &histRange, true, false);
    calcHist(&s_plane, 1, 0, Mat(), s_hist, 1, &histSize, &histRange, true, false);
    calcHist(&v_plane, 1, 0, Mat(), v_hist, 1, &histSize, &histRange, true, false);

    // �ҳ��� 0 ֮����ִ���������ɫ
    int h_maxVal = 0, s_maxVal = 0, v_maxVal = 0;
    float h_maxNum = 0, s_maxNum = 0, v_maxNum = 0;
    for (int r = 1; r < h_hist.rows; r++) {
        float* h_pointer = h_hist.ptr<float>(r);
        float* s_pointer = s_hist.ptr<float>(r);
        float* v_pointer = v_hist.ptr<float>(r);
        for (int c = 0; c < h_hist.cols; c++) {
            if (h_maxNum < h_pointer[c]) {
                h_maxNum = h_pointer[c];
                h_maxVal = cvRound(r / 180.0 * 255.0);
            }
            if (s_maxNum < s_pointer[c]) {
                s_maxNum = s_pointer[c];
                s_maxVal = r;
            }
            if (v_maxNum < v_pointer[c]) {
                v_maxNum = v_pointer[c];
                v_maxVal = r;
            }
        }
    }

    // ����ʱ����м���
    //cout<<"h = "<<h_maxVal<<endl;
    //cout<<"s = "<<s_maxVal<<endl;
    //cout<<"v = "<<v_maxVal<<endl;

    // �ж���ɫ
    string color;
    if (v_maxVal < 46) {
        color = "black";
    }
    else if (v_maxVal < 221 && s_maxVal < 43) {
        color = "gray";
    }
    else if (v_maxVal >= 221 && s_maxVal < 43) {
        color = "white";
    }
    else {
        if (h_maxVal <= 15 || h_maxVal > 222) {
            color = "red";
        }
        else if (h_maxVal > 15 && h_maxVal <= 48) {
            color = "yellow";
        }
        else if (h_maxVal > 48 && h_maxVal <= 109) {
            color = "green";
        }
        else if (h_maxVal > 109 && h_maxVal <= 140) {
            color = "cyan";
        }
        else if (h_maxVal > 140 && h_maxVal <= 175) {
            color = "blue";
        }
        else if (h_maxVal > 175 && h_maxVal <= 222) {
            color = "purple";
        }
    }

    return color;
}