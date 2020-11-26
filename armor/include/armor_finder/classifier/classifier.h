#ifndef _CLASSIFIER_H_
#define _CLASSIFIER_H_
#include <NumberDetector.h>
#include <runtime.h>
using namespace std;
class Classifier {
   public:
    bool state;
    NumberDetector detector;
    int run_cnt;
    double run_time;
    Classifier();
    explicit operator bool() const { return state; }
    int operator()(const cv::Mat &image);
};

#endif