#ifndef _NUMBERDETECTOR_H
#define _NUMBERDETECTOR_H

#ifdef WITH_TINY_TENSORRT 
#include<Trt.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
using namespace std;

using namespace cv;

class NumberDetector{
private:
    Trt* onnx_net;
public:
    int status;
    double TOTAL_INFER_TIME;
    NumberDetector();
    void Initialize(vector<vector<float>> calibratorData = vector<vector<float>>(),bool calibrate = false);
    pair<int,float> detect(const Mat& img_raw,bool init = false);
    ~NumberDetector(){
        if(onnx_net) delete onnx_net;
    }
};

#else
#include <opencv2/opencv.hpp>
#include <MNN/Interpreter.hpp>
#include <opencv2/dnn.hpp>
#include <runtime.h>

using namespace std;
using namespace MNN;
using namespace cv;

class NumberDetector{
private:
    Interpreter* interpreter;
    Session* session;
    Tensor* input_tensor;
    Tensor* nchwTensor;
    const char* model_name = "RM.mnn";
public:
    int status;
    double TOTAL_INFER_TIME;
    NumberDetector();
    void Initialize();
    pair<int,float> detect(const Mat& img_raw);
};
#endif

#endif //NUMBERDETECTOR_H
