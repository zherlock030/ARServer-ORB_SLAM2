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
#include<chrono>

#include<opencv2/core/core.hpp>

#include<System.h>
//#include "Osmap.h"
#include <unistd.h>

using namespace std;

void LoadImages(const string &strImagePath, const string &strPathTimes,
                vector<string> &vstrImages, vector<double> &vTimeStamps);

int main(int argc, char **argv) {
    if (argc != 5) {
        cerr << endl << "Usage: ./mono_tum path_to_vocabulary path_to_settings path_to_image_folder path_to_times_file"
             << endl;
        return 1;
    }

    // Retrieve paths to images
    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
    LoadImages(string(argv[3]), string(argv[4]), vstrImageFilenames, vTimestamps);

    int nImages = vstrImageFilenames.size();

    if (nImages <= 0) {
        cerr << "ERROR: Failed to load images" << endl;
        return 1;
    }

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1], argv[2], ORB_SLAM2::System::MONOCULAR, false);
    //ORB_SLAM2::Osmap osmap = ORB_SLAM2::Osmap(SLAM);

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;

    clock_t last_time = 0;
    int count = 0;

    // Main loop
    int main_error = 0;
    std::thread runthread([&]() {  // Start in new thread
        cv::Mat im;

        for (int ni = 0; ni < nImages; ni++) {
            // Read image from file
            im = cv::imread(vstrImageFilenames[ni], CV_LOAD_IMAGE_UNCHANGED);
            double tframe = vTimestamps[ni];

            if (im.empty()) {
                cerr << endl << "Failed to load image at: "
                     << vstrImageFilenames[ni] << endl;
                main_error = 1;
                return;
//            return 1;
            }

#ifdef COMPILEDWITHC11
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
#else
            std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
#endif
            // Pass the image to the SLAM system
            if (last_time == 0) {
                last_time = clock();
            } else if (clock() - last_time >= 2 * CLOCKS_PER_SEC) {
                double elapsed = (double(clock() - last_time) / double(CLOCKS_PER_SEC));
                double flush_rate = count / elapsed;
//                cout << flush_rate << endl;
                last_time = clock();
                count = 0;
            }
            count++;
            clock_t last,now;
            last = clock();
            SLAM.TrackMonocular(im, tframe);
            now = clock();
            cout<<double(now-last)*1000/CLOCKS_PER_SEC<<endl;
            if (ni == 0) {
//                osmap.mapLoad("myFirstMap.yaml");
            }

#ifdef COMPILEDWITHC11
            std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
#else
            std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
#endif

            double ttrack = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

            vTimesTrack[ni] = ttrack;

            // Wait to load the next frame
            double T = 0;
            if (ni < nImages - 1)
                T = vTimestamps[ni + 1] - tframe;
            else if (ni > 0)
                T = tframe - vTimestamps[ni - 1];

            if (ttrack < T)
                usleep((T - ttrack) * 1e6);
        }

        cout << "End of for loop" << endl;
        // SLAM.Shutdown();
    }); // End the thread

    // Start the visualization thread
//    SLAM.StartViewer();

    cout << "Viewer started, waiting for thread." << endl;
    runthread.join();
    if (main_error != 0)
        return main_error;
    cout << "Tracking thread joined..." << endl;

    // Stop all threads
//    osmap.mapSave("myFirstMap");
    SLAM.Shutdown();

    // Tracking time statistics
    sort(vTimesTrack.begin(), vTimesTrack.end());
    float totaltime = 0;
    for (int ni = 0; ni < nImages; ni++) {
        totaltime += vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time: " << vTimesTrack[nImages / 2] << endl;
    cout << "mean tracking time: " << totaltime / nImages << endl;

    // Save camera trajectory
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    return 0;
}

void LoadImages(const string &strImagePath, const string &strPathTimes,
                vector<string> &vstrImages, vector<double> &vTimeStamps) {
    ifstream fTimes;
    fTimes.open(strPathTimes.c_str());
    vTimeStamps.reserve(5000);
    vstrImages.reserve(5000);
    while (!fTimes.eof()) {
        string s;
        getline(fTimes, s);
        if (!s.empty()) {
            stringstream ss;
            ss << s;
            vstrImages.push_back(strImagePath + "/" + ss.str() + ".png");
            double t;
            ss >> t;
            vTimeStamps.push_back(t / 1e9);

        }
    }
}
