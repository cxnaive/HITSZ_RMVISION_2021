#include <armor_finder/armor_finder.h>
#include <runtime.h>
#include <cmath>
#include <armor_finder/classifier/classifier.h>
#include <rmserial.h>
#define WITH_COUNT_FPS


static bool sendTarget(RmSerial &serial, double x, double y, double z, uint16_t shoot_delay,double y_offset = 0) {
    static short x_tmp, y_tmp, z_tmp;
    uint8_t buff[10];
    x_tmp = static_cast<short>(x);
    y_tmp = static_cast<short>(y);
    z_tmp = static_cast<short>(z);
#ifdef WITH_COUNT_FPS
    static time_t last_time = time(nullptr);
    static int fps;
    time_t t = time(nullptr);
    if (last_time != t) {
        last_time = t;
        LOG(INFO) << "Armor: fps:" << fps << ", (" << x << "," << y << "," << z << ")";
        fps = 0;
    }
    fps += 1;
#endif


    buff[0] = 's';
    buff[1] = static_cast<char>((x_tmp >> 8) & 0xFF);
    buff[2] = static_cast<char>((x_tmp >> 0) & 0xFF);
    buff[3] = static_cast<char>((y_tmp >> 8) & 0xFF);
    buff[4] = static_cast<char>((y_tmp >> 0) & 0xFF);
    buff[5] = static_cast<char>((z_tmp >> 8) & 0xFF);
    buff[6] = static_cast<char>((z_tmp >> 0) & 0xFF);
    buff[7] = static_cast<char>((shoot_delay >> 8) & 0xFF);
    buff[8] = static_cast<char>((shoot_delay >> 0) & 0xFF);
    buff[9] = 'e';
    //   cout << (short)(buff[3]<<8 | buff[4]) << endl;
    return serial.send_data(buff, sizeof(buff));
}

bool ArmorFinder::sendBoxPosition(uint16_t shoot_delay,double dist) {
    if (dist == -1 && target_box.rect == cv::Rect2d()) return false;
    if (shoot_delay) {
        LOG(INFO) << "next box" << shoot_delay << " ms";
    }

    auto rect = target_box.rect;
    if(dist == -1) dist = DISTANCE_HEIGHT / rect.height;
    double y_offset = 300*50 / dist;
    double dx = rect.x + rect.width / 2 - IMAGE_CENTER_X;
    double dy = rect.y + rect.height / 2 - IMAGE_CENTER_Y;
    dy -= y_offset;
    double yaw = atan(dx / FOCUS_PIXAL) * 180 / PI;                          //以下几句为根据二维图像偏移给云台的值
    double pitch = atan(dy / FOCUS_PIXAL) * 180 / PI;

    //if(dist != 0) cout << "y_offset: " << y_offset << endl;
    //cout<< "pitch: " << pitch << " dist: "<<dist<<endl;
    return sendTarget(serial, yaw, -pitch, dist, shoot_delay,y_offset);
}


