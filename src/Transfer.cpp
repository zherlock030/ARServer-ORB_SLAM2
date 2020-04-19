//
// Created by 范宏昌 on 2020/4/5.
//

#include "Transfer.h"
#include <opencv2/core/core.hpp>

Status Transfer::GetViewMatrix(ServerContext *context, const Request *request, Matrix *response) {
    cout<<"Transfer start"<<endl;
    if (!system->mpTracker->mCurrentFrame.mTcw.empty()) {
        cv::Mat& Tcw = system->mpTracker->mCurrentFrame.mTcw;
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
        cout<<"success: transfering pose "<<Tcw<<endl;
    }else{
        cout<<"error: pose not available!"<<endl;
        response->set_update(false);
    }

    return Status::OK;
}
