/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#include<iostream>
#include<algorithm>
#include<fstream>
#include<ctime>
#include "Osmap.h"
#include "TwoLineIn3D.h"
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<System.h>
#include "viewmatrix.pb.h"
#include "viewmatrix.grpc.pb.h"
#include "Transfer.h"
#include <grpcpp/grpcpp.h>
#include <mutex>

//using grpc::ServerBuilder ;

using namespace std;
using namespace cv;

float fx;
float fy;
float cx;
float cy;
float F = 10;

int main(int argc, char **argv) {
    if (argc != 4) {
        cerr << endl << "Usage: ./mono_tum path_to_vocabulary path_to_settings url_of_the_ip_camera" << endl;
        return 1;
    }
    cv::FileStorage fsSettings(argv[2], cv::FileStorage::READ);
    if (!fsSettings.isOpened()) {
        cerr << "Failed to open settings file at: " << argv[2] << endl;
        exit(-1);
    }
    fx = fsSettings["Camera.fx"];
    fy = fsSettings["Camera.fy"];
    cx = fsSettings["Camera.cx"];
    cy = fsSettings["Camera.cy"];
    bool finish = false;

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1], argv[2], ORB_SLAM2::System::MONOCULAR, true);
    ORB_SLAM2::Osmap osmap = ORB_SLAM2::Osmap(SLAM);
    string server_address = "0.0.0.0:50051";
    Transfer service(&SLAM);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<grpc::Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << endl;

    std::thread serverthread([&]() {
        server->Wait();
    });

    VideoCapture cap;
    Mat frame;
    mutex bufferMutex;
    clock_t last_time=0;
    int count=0;
    std::thread bufferthread([&]() {
        int key = 0;
        while (!finish) {
            if (cap.open(string(argv[3])),CV_CAP_DSHOW) {
                cout << "video stream reconnected!" << endl;
                cap.set(CV_CAP_PROP_FPS,30);
                while (true) {
                    {
//                        unique_lock<mutex> lock(bufferMutex);
                        cap.read(frame);
                        count += 1;
                        if (last_time == 0) {
                            last_time = clock();
                        } else if (clock() - last_time >= 2 * CLOCKS_PER_SEC) {
                            double flush_rate = count / (double(clock() - last_time) / double(CLOCKS_PER_SEC));
                            cout <<  flush_rate << endl;
                            last_time = clock();
                            count = 0;
                        }
                    }
                    if (frame.empty()) {
                        key++;
                        if (key >= 25) {
                            key=0;
                            break;
                        } else {
                            continue;
                        }
                    } else {
                        key = 0;
                    }
                }
            }
            cout << "video stream disconnected! sleep 1s." << endl;
            sleep(1);
        }
    });


    std::thread runthread([&]() {  // Start in new thread
        Mat C_Tcw;

        struct timespec tn;
        double tframe;

        cout << endl << "-------" << endl;
        cout << "Start processing sequence ..." << endl;
        int inited = 0;

        while (!finish) {
            Mat temp;
            {
//                unique_lock<mutex> lock(bufferMutex);
//                frame.copyTo(temp);
            }
            clock_gettime(CLOCK_REALTIME, &tn);
            tframe = double(tn.tv_sec) + double(tn.tv_nsec) / 1e9;
            if(!frame.empty()){
                C_Tcw = SLAM.TrackMonocular(frame, tframe);
                if (inited == 0) {
//                osmap.mapLoad("myHome.yaml");
                    inited++;
                }
            }
        }
    });
    // Start the visualization thread
    SLAM.StartViewer();
    finish = true;
    runthread.join();
    bufferthread.join();
    server->Shutdown();
    serverthread.join();
    cout << "Tracking thread joined..." << endl;
    // Stop all threads
    osmap.mapSave("myHome");
    SLAM.Shutdown();


    return 0;
}
