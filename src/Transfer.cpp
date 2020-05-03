//
// Created by 范宏昌 on 2020/4/5.
//

#include "Transfer.h"
#include <opencv2/core/core.hpp>
#include <ctime>

int Transfer::count = 0;
clock_t  Transfer::last_time = 0;

Status Transfer::GetViewMatrix(ServerContext *context, const Request *request, Matrix *response) {
//    count += 1;
//    if (last_time == 0) {
//        last_time = clock();
//    } else if (clock() - last_time >= 2 * CLOCKS_PER_SEC) {
//        double flush_rate = count / (double(clock() - last_time) / double(CLOCKS_PER_SEC));
//        cout <<  flush_rate << endl;
//        last_time = clock();
//        count = 0;
//    }
//    cout<<"Transfer start"<<endl;
//    if (!system->mpTracker->mCurrentFrame.mTcw.empty()) {
//        cv::Mat& Tcw = system->mpTracker->mCurrentFrame.mTcw;
//        response->set_r0c0(Tcw.at<float>(0, 0));
//        response->set_r0c1(Tcw.at<float>(0, 1));
//        response->set_r0c2(Tcw.at<float>(0, 2));
//        response->set_r0c3(Tcw.at<float>(0, 3));
//        response->set_r1c0(Tcw.at<float>(1, 0));
//        response->set_r1c1(Tcw.at<float>(1, 1));
//        response->set_r1c2(Tcw.at<float>(1, 2));
//        response->set_r1c3(Tcw.at<float>(1, 3));
//        response->set_r2c0(Tcw.at<float>(2, 0));
//        response->set_r2c1(Tcw.at<float>(2, 1));
//        response->set_r2c2(Tcw.at<float>(2, 2));
//        response->set_r2c3(Tcw.at<float>(2, 3));
//        response->set_r3c0(Tcw.at<float>(3, 0));
//        response->set_r3c1(Tcw.at<float>(3, 1));
//        response->set_r3c2(Tcw.at<float>(3, 2));
//        response->set_r3c3(Tcw.at<float>(3, 3));
//        response->set_update(true);
//        cout<<"success: transfering pose "<<Tcw<<endl;
//    }else{
//        cout<<"error: pose not available!"<<endl;
//        response->set_update(false);
//    }
    if (!system->mpViewer->modelViewMatrix.empty()) {
        cv::Mat Tcw = system->mpViewer->modelViewMatrix;
        response->set_r0c0(Tcw.at<float>(0, 0));
        response->set_r0c1(Tcw.at<float>(0, 1));
        response->set_r0c2(Tcw.at<float>(0, 2));
        response->set_r0c3(Tcw.at<float>(0, 3));
        response->set_r1c0(Tcw.at<float>(1, 0));
        response->set_r1c1(Tcw.at<float>(1, 1));
        response->set_r1c2(Tcw.at<float>(1, 2));
        response->set_r1c3(Tcw.at<float>(1, 3));
        response->set_r2c0(Tcw.at<float>(2, 0));
        response->set_r2c1(Tcw.at<float>(2, 1));
        response->set_r2c2(Tcw.at<float>(2, 2));
        response->set_r2c3(Tcw.at<float>(2, 3));
        response->set_r3c0(Tcw.at<float>(3, 0));
        response->set_r3c1(Tcw.at<float>(3, 1));
        response->set_r3c2(Tcw.at<float>(3, 2));
        response->set_r3c3(Tcw.at<float>(3, 3));
        response->set_update(true);
//        cout<<"success: transfering pose "<<Tcw<<endl;
    } else {
//        cout<<"error: pose not available!"<<endl;
        response->set_update(false);
    }

    return Status::OK;
}
