#include<glog/logging.h>
#include<json/json.h>
#include<constants.h>
#include<fstream>
#ifndef H_RMCONFIG
#define H_RMCONFIG

//程序运行时各参数设置类
class RmConfig{
public:
    const char* configPath = "rmconfig.json";
    //config
    bool show_origin = false;
    bool show_armor_box = false;
    bool show_light_box = false;
    bool show_class_id = false;
    bool wait_uart = false;
    bool save_video = false;
    bool show_light_blobs = false;
    bool save_labelled_boxes = false;

    //DATA
    int ARMOR_CAMERA_EXPOSURE = 4000;
    int ARMOR_CAMERA_GAIN = 7;
    int ENERGY_CAMERA_EXPOSURE = 4000;
    int ENERGY_CAMERA_GAIN = 5;
    int ENEMY_COLOR = ENEMY_RED;
    int ANTI_TOP = 0;

    void init_from_file();
    void write_to_file();
};

#endif