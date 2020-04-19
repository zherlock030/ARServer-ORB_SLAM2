//
// Created by 范宏昌 on 2020/3/23.
//

#include "Locater.h"
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
namespace ORB_SLAM2 {


void Locater::Shoot(Mat C_Tcw) {
    Mat C_Rcw;
    Mat C_Rwc;
    Mat C_tcw;
    Mat C_Ow;
//            cout<<C_Tcw<<endl;
    Mat Pch = Mat(4, 1, CV_32F);
//    Pch.at<float>(0, 0) = (640 - cx) * F / fx;
//    Pch.at<float>(0, 1) = (480 - cy) * F / fy;
    Pch.at<float>(0, 0) = 0;
    Pch.at<float>(0, 1) = 0;
    Pch.at<float>(0, 2) = F;
    Pch.at<float>(0, 3) = 1;

//    C_Rcw = C_Tcw.rowRange(0, 3).colRange(0, 3);
//    C_Rwc = C_Rcw.t();
//    C_tcw = C_Tcw.rowRange(0, 3).col(3);
//    C_Ow = -C_Rcw.t() * C_tcw;
    Mat C_Twc = C_Tcw.inv();
    Mat Pwh = C_Twc * Pch;
    cout<<Pch<<endl;
    cout<<Pwh<<endl;
    cout<<C_Twc<<endl;
//    Mat Pwh = C_Tcw * Pch;


    if (!counter) {
        counter++;
        cout << "set line A" << endl;
        calculater.SetLineA(C_Twc.at<double>(0, 3), C_Twc.at<double>(1, 3), C_Twc.at<double>(2, 3),
                            Pwh.at<double>(0, 0), Pwh.at<double>(0, 1), Pwh.at<double>(0, 2));
    } else {
        counter = 0;
        cout << "set line B" << endl;
        calculater.SetLineB(C_Twc.at<double>(0, 3), C_Twc.at<double>(1, 3), C_Twc.at<double>(2, 3),
                            Pwh.at<double>(0, 0), Pwh.at<double>(0, 1), Pwh.at<double>(0, 2));

        calculater.GetDistance();
        shared_ptr<Point3d> point = make_shared<Point3d>(calculater.Pon_x, calculater.Pon_y, calculater.Pon_z);
        cout << "Pon at point: " << *point << endl;
        targets.push_back(point);
    }
}

Locater::Locater(string strSettingsFile) {
    counter = 0;
    cv::FileStorage fSettings(strSettingsFile, cv::FileStorage::READ);
    if (!fSettings.isOpened()) {
        cerr << "Failed to open settings file at: " << strSettingsFile << endl;
        exit(-1);
    }
    fx = fSettings["Camera.fx"];
    fy = fSettings["Camera.fy"];
    cx = fSettings["Camera.cx"];
    cy = fSettings["Camera.cy"];
}
}