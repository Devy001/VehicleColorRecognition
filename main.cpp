/*
*  ������ɫʶ��
*/

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/highgui/highgui.hpp>

#include "VehicleColorRecognition.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    int imgNum = 1; // ���Գ���������
    char filename[200];

    ofstream fout;
    fout.open("result.txt"); // ����ÿ��������ɫ

    double t_start = getTickCount();

    for (int i = 1; i <= imgNum; i++) {
        sprintf(filename, "images/%d.jpg", i); // ����ͼƬ��

        Mat rgbImg = imread(filename);
        string color = recognizeVehicleColor(rgbImg); // ������ɫʶ��
        fout << i << ".jpg    " << color << endl;
    }

    double t_end = getTickCount();
    double t_cost = (t_end - t_start) / getTickFrequency();
    cout << "Cost time = " << t_cost << endl;

    fout.close();

    return 0;
}