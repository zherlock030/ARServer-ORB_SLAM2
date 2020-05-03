//
// Created by 范宏昌 on 2020/4/5.
//

#ifndef ORB_SLAM2_TRANSFER_H
#define ORB_SLAM2_TRANSFER_H

#include "viewmatrix.grpc.pb.h"
#include "viewmatrix.pb.h"
#include "System.h"
using namespace grpc;
using namespace viewmatrix;
class Transfer final: public ViewTrans::Service {
public:
    ORB_SLAM2::System* system;

    Transfer(ORB_SLAM2::System* system):system(system){}

    Status GetViewMatrix(ServerContext* context, const Request* request, Matrix* response);

    static int count;
    static clock_t last_time;
};


#endif //ORB_SLAM2_TRANSFER_H
