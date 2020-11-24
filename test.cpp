#include<rmtime.h>
#include<iostream>
#include<NumberDetector.h>

int main(){
    RmTime time;
    time.init();
    double s = time.milliseconds();
    time.sleep(1000);
    double e = time.milliseconds();
    std::cout << e - s << std::endl;
    NumberDetector detector;
    detector.Initialize();
}