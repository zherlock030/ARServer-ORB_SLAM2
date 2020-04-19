//
// Created by 范宏昌 on 2020/3/23.
//

#ifndef ORB_SLAM2_LOCATER_H
#define ORB_SLAM2_LOCATER_H

#include <string>
#include "TwoLineIn3D.h"
#include <opencv2/opencv.hpp>
namespace ORB_SLAM2{
using namespace std;
using namespace cv;

class Locater {
public:
    TwoLineIn3D calculater;
    int counter;

    vector<shared_ptr<Point3d>>targets;

    Locater(string strSettingsFile);
    void Shoot(Mat C_Tcw);

private:
    float fx;
    float fy;
    float cx;
    float cy;
    float F = 1;
};
}

#endif //ORB_SLAM2_LOCATER_H
