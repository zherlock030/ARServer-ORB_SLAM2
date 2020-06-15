#include "InstanceDetect.h"


using namespace std;

namespace ORB_SLAM2
{
InstanceDetect::InstanceDetect(Map* pMap):mpMap(pMap),mbResetRequested(false), mbFinishRequested(false),mbFinished(true)
{
  modelpath = "faster.pt";
  long start = time_in_ms();
  model = torch::jit::load(modelpath);
  long end = time_in_ms();
  cout << "it took " << end - start << " ms to load the model" << endl;
  torch::jit::getProfilingMode() = false;
  torch::jit::getExecutorMode() = false;
  torch::jit::setGraphExecutorOptimize(false);

  ifstream f;
  labelpath = "labels.txt";
  f.open(labelpath);
  string str;
  while (std::getline(f,str)){
    labels.push_back(str);
  }
  cout << "we get " << labels.size() << " labels" << endl;
}


long InstanceDetect::time_in_ms(){
      struct timeval t;
      gettimeofday(&t, NULL);
      long time_ms = ((long)t.tv_sec)*1000+(long)t.tv_usec/1000;
      return time_ms;
}

void InstanceDetect::initmodel(){
  modelpath = "faster.pt";
  long start = time_in_ms();
  model = torch::jit::load(modelpath);
  long end = time_in_ms();
  cout << "it took " << end - start << " ms to load the model" << endl;
  torch::jit::getProfilingMode() = false;
  torch::jit::getExecutorMode() = false;
  torch::jit::setGraphExecutorOptimize(false);

  ifstream f;
  labelpath = "labels.txt";
  f.open(labelpath);
  string str;
  while (std::getline(f,str)){
    labels.push_back(str);
  }
  cout << "we get " << labels.size() << " labels" << endl;
}

void InstanceDetect::Run()
{
      mbFinished =false;
      while(1)
      {
      //  cout << "detection thread running" << endl;
      if (CheckNewDetectFrames()){
        //cout << "get in check new frames " << endl;
        detect(midFrameQueue.front());
        midFrameQueue.pop_front();
        //getchar();
        //getchar();
        }

      //cout << "before ResetIfRequested" << endl;
      ResetIfRequested();
      //cout << "after ResetIfRequested" << endl;

      if(CheckFinish()){
        //cout << "into CheckFinish()" << endl;
          break;
        }
        //cout << "before usleep" << endl;
      usleep(5000);//usleep(5000);
      //cout << "after usleep" << endl;
      }
SetFinish();

}

void InstanceDetect::InsertFrame(cv::Mat* im)
{
    unique_lock<mutex> lock(mMutexInstanceDetectQueue);
    midFrameQueue.push_back(im);
}

bool InstanceDetect::CheckNewDetectFrames(){
  unique_lock<mutex> lock(mMutexInstanceDetectQueue);
  return(!midFrameQueue.empty());
}

void InstanceDetect::ResetIfRequested()
{
    unique_lock<mutex> lock(mMutexReset);
    if(mbResetRequested)
    {
        midFrameQueue.clear();
        mbResetRequested=false;
    }
}


void InstanceDetect::RequestFinish()//要主动调用，来结束线程。
{
    unique_lock<mutex> lock(mMutexFinish);
    mbFinishRequested = true;
}

bool InstanceDetect::CheckFinish()
{
    unique_lock<mutex> lock(mMutexFinish);
    return mbFinishRequested;
}

void InstanceDetect::SetFinish()
{
    unique_lock<mutex> lock(mMutexFinish);
    mbFinished = true;
}



//void InstanceDetect::detect(const cv::Mat &im){

//  std::vector<torch::jit::IValue> inputs;
//  inputs.push_back(torch::ones({1, 3, 224, 224}));

  // Execute the model and turn its output into a tensor.
//  at::Tensor output = model.forward(inputs).toTensor();

//  std::cout << output.slice(/*dim=*/1, /*start=*/0, /*end=*/5) << '\n';

//  std::cout << "ok\n";



void InstanceDetect::detect(cv::Mat* im){
  
  torch::jit::getProfilingMode() = false;
  torch::jit::getExecutorMode() = false;
  torch::jit::setGraphExecutorOptimize(false);
  cv::Mat frame = *im;
  //im.copyTo(frame);
  cout << " into func detect" << endl;
  cvtColor(frame, frame, CV_BGR2RGB);
  cout << " cvt color good" << endl;
  frame.convertTo(frame, CV_32FC3, 1.0f / 255.0f);
  cout << " preprocess good" << endl;
  auto tensor_image = torch::from_blob(frame.data, {frame.rows, frame.cols, frame.channels()});
  cout << " to tensor good" << endl;
  //tensor_image = torch::ones({480, 640, 3});
  tensor_image = tensor_image.permute({2, 0, 1});

  long start = time_in_ms();
  c10::List<Tensor> images = c10::List<Tensor>({tensor_image});
  cout << "list tensor image is good" <<endl;

  std::vector<torch::jit::IValue> inputs;
  inputs.emplace_back(images);
  cout << "inputs is good" <<endl;
  torch::jit::IValue output = model.forward(inputs);
  long end = time_in_ms();
  cout << "it took " << end - start << " ms to run the model once" << endl;
  auto out1 = output.toTuple();
  auto dets1 = out1->elements().at(1).toList().get(0).toGenericDict() ;
  at::Tensor scores = dets1.at("scores").toTensor();
  at::Tensor labels = dets1.at("labels").toTensor();
  at::Tensor boxes = dets1.at("boxes").toTensor();

  float score = scores[0].item().toFloat();
  cout <<  "score is " << score  <<  endl;


  int label = labels[0].item().toInt();
  cout <<  "labels is " << label <<  endl;
  //cout <<  "labels is " << labels[label] <<  endl;

  float c1 = boxes[0][0].item().toFloat();
  float c2 = boxes[0][1].item().toFloat();
  float c3 = boxes[0][2].item().toFloat();
  float c4 = boxes[0][3].item().toFloat();

  cout << "box 0 is " << c1 << "---" << c2 << "---" << c3 << "---" << c4 << "---" << endl;
    //getchar();
    //getchar();
    for (int i = 0; i< 200; i++){
      cout << i << endl;
    }
}




}
