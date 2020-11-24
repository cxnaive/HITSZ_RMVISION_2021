#ifndef _NUMBERDETECTOR_H
#define _NUMBERDETECTOR_H
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <MNN/Interpreter.hpp>
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
#endif //NUMBERDETECTOR_H
