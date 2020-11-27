#ifndef _H_RUNTIME_
#define _H_RUNTIME_

#include <RoundQueue.h>
#include <rmtime.h>
#include <rmconfig.h>
#include <glog/logging.h>
#include <constants.h>
#include <opencv2/opencv.hpp>
#include <armor_finder/armor_box.h>


void saveVideos(cv::Mat& img,std::string prefix);
double getPointLength(const cv::Point2f &p);
std::vector<cv::Point2f> getArmorPoints(ArmorBox armor);
void armorSolvePnP(ArmorBox armor,cv::Point3d &rotate_eular,cv::Point3d &translation);
extern RmConfig config;
extern RmTime rmTime;
extern cv::Mat src; 
#endif
