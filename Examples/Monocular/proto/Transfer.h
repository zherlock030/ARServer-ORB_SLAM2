//
// Created by 范宏昌 on 2020/4/5.
//

#ifndef ORB_SLAM2_TRANSFER_H
#define ORB_SLAM2_TRANSFER_H

#include "ARConnection.grpc.pb.h"
#include "ARConnection.pb.h"
#include <condition_variable>
#include "System.h"
using namespace grpc;
using namespace ARConnection;

class Transfer final: public ARConnectionService::Service {
public:
    Mat frame;
    Mat Twc;
    mutex bufferMutex,mMutexCamera;
    condition_variable mCondVar;
    bool updated = false;
    bool reset = false;

    Transfer();

    void SetCurrentCameraPose(const cv::Mat &Tcw);
    Status GetViewMatrix(ServerContext* context, const Request* request, MatrixBlock* response) override ;
    Status UploadImage(ServerContext* context, ServerReader< ImageBlock>* reader, Response* response) override;
    Status RequestReset(ServerContext* context, const Request* request, Response* response);
    static int count;
    static clock_t last_time;
};


#endif //ORB_SLAM2_TRANSFER_H
