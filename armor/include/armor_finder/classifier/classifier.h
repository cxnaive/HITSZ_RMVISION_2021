#ifndef _CLASSIFIER_H_
#define _CLASSIFIER_H_
#include <NumberDetector.h>
#include <runtime.h>
using namespace std;
class Classifier{

private:
public:
    bool state;
    NumberDetector detector;
    int run_cnt;
    double run_time;
    Classifier(){
        detector.Initialize();
        state = detector.status == 1;
        run_time = 0;
        run_cnt = 0;
    }
    explicit operator bool() const{
        return state;
    }
    int operator()(const cv::Mat &image){
        double time_bg = rmTime.milliseconds();
        auto dat = detector.detect(image);
        double time_ed = rmTime.milliseconds();
        run_cnt += 1;
        run_time += time_ed - time_bg;
        if(dat.second < 0.8) return 0;
        else return dat.first + 1;
    }
};

#endif