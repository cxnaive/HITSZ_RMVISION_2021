#include <armor_finder/classifier/classifier.h>

Classifier::Classifier() {
    detector.Initialize();
    state = detector.status == 1;
    run_time = 0;
    run_cnt = 0;
}

int Classifier::operator()(const cv::Mat &image) {
    double time_bg = rmTime.milliseconds();
    auto dat = detector.detect(image);
    double time_ed = rmTime.milliseconds();
    run_cnt += 1;
    run_time += time_ed - time_bg;
    if (dat.second < 0.9)
        return 0;
    else
#ifdef WITH_TINY_TENSORRT
        return dat.first;
#else
        return dat.first + 1;
#endif
}