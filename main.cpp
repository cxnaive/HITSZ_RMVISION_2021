#include <camera/cam_wrapper.h>
#include <glog/logging.h>
#include <rmconfig.h>
#include <rmtime.h>
#include <armor_finder/armor_finder.h>
#include <csignal>
#include <rmserial.h>
#include <mutex>
#include <opencv2/opencv.hpp>

using namespace std;
//检测关闭程序键盘中断
static volatile int keepRunning = 1;

void sig_handler(int sig) {
    if (sig == SIGINT) {
        keepRunning = 0;
    }
}

//程序运行时信息
RmConfig config;
//程序运行时时间类
RmTime rmTime;
//串口实例
RmSerial rmSerial;
//摄像头实例
Camera* cam = nullptr;
//ArmorFinder实例
ArmorFinder* armor_finder;
static void OnInit(const char* cmd) {
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(cmd);
    rmTime.init();

    config.init_from_file();
    
    rmSerial.init();

    cam = new Camera(1);
    cam->init();
    cam->setParam(config.ARMOR_CAMERA_EXPOSURE, config.ARMOR_CAMERA_GAIN);
    cam->start();
    
    armor_finder = new ArmorFinder(config.ENEMY_COLOR,rmSerial,config.ANTI_TOP);
}

static void OnClose() { 
    config.write_to_file(); 
    //delete cam;
    //delete armor_finder;
}

int main(int argc, char** argv) {
    signal(SIGINT, sig_handler);
    OnInit(argv[0]);
    cv::Mat src;
    int frame_cnt = 0;
    double stime = rmTime.seconds();
    while (keepRunning) {
        cam->read(src);
        if(config.show_origin){
            cv::imshow("origin",src);
            cv::waitKey(1);           
        } 
        armor_finder->run(src);
    }
    LOG(INFO) << "exiting...";
    OnClose();
    return 0;
}