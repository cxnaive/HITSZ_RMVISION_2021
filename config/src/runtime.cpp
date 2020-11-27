#include <runtime.h>
#include <map>
#include <cmath>

double getPointLength(const cv::Point2f &p) {
    return sqrt(p.x * p.x + p.y * p.y);
}
std::vector<cv::Point2f> getArmorPoints(ArmorBox armor){
    if(armor.light_blobs.size() != 2){
        auto o  = armor.rect.tl();
        float w = armor.rect.width,h = armor.rect.height;
        return {cv::Point2f(o.x,o.y + h),o,cv::Point2f(o.x + w,o.y),cv::Point2f(o.x + w,o.y + h)};
    }
    cv::Point2f points[4],tmp[4];
    auto rectL = armor.light_blobs.at(0).rect;
    auto rectR = armor.light_blobs.at(1).rect;
    if(rectL.center.x > rectR.center.x) std::swap(rectL,rectR);
    if(rectL.size.width > rectL.size.height){
        rectL.size.width *= 2;
        rectL.points(tmp);
        points[0] = tmp[1];
        points[1] = tmp[2];
    }
    else{
        rectL.size.height *= 2;
        rectL.points(tmp);
        points[0] = tmp[0];
        points[1] = tmp[1];
    }

    if(rectR.size.width > rectR.size.height){
        rectR.size.width *= 2;
        rectR.points(tmp);
        points[2] = tmp[3];
        points[3] = tmp[0];
    }
    else{
        rectR.size.height *= 2;
        rectR.points(tmp);
        points[2] = tmp[2];
        points[3] = tmp[3];
    }
    return {points[0],points[1],points[2],points[3]};
}
cv::VideoWriter initVideoWriter(const std::string &filename_prefix) {
    cv::VideoWriter video;
    std::string file_name = filename_prefix + ".avi";
    video.open(file_name, cv::VideoWriter::fourcc('P', 'I', 'M', 'I'), 90, cv::Size(640, 480), true);
    return video;
}
std::map<std::string,cv::VideoWriter> video_writers;
cv::VideoWriter getVideoWriter(std::string prefix){
    auto iter = video_writers.find(prefix);
    if(iter != video_writers.end()) return iter->second;
    cv::VideoWriter now = initVideoWriter("/video/"+prefix);
    video_writers[prefix] = now;
    return now;
}
void saveVideos(cv::Mat& img,std::string prefix){
    if(img.empty()) return;
    cv::VideoWriter writer = getVideoWriter(prefix);
    writer.write(img);
}
void armorSolvePnP(ArmorBox armor,cv::Point3d &rotate_eular,cv::Point3d &translation) {
    static double x = config.ARMOR_W / 2.0;
    static double y = config.ARMOR_H / 2.0;
    static std::vector<cv::Point3f> world_points = {cv::Point3f(-x,-y,0),cv::Point3f(-x,y,0),cv::Point3f(x,y,0),cv::Point3f(x,-y,0)};
    cv::Mat Rvec,Tvec,rotM,invrotM,P;
    cv::solvePnP(world_points,getArmorPoints(armor),config.camConfig.mtx,config.camConfig.dist,Rvec,Tvec,false,cv::SOLVEPNP_ITERATIVE);

    cv::Rodrigues(Rvec,rotM);
    double theta_x = atan2(rotM.at<double>(2, 1), rotM.at<double>(2, 2));
    double theta_y = atan2(-rotM.at<double>(2, 0),
    sqrt(rotM.at<double>(2, 1)*rotM.at<double>(2, 1) + rotM.at<double>(2, 2)*rotM.at<double>(2, 2)));
    double theta_z = atan2(rotM.at<double>(1, 0), rotM.at<double>(0, 0));
    rotate_eular = {theta_x * (180 / PI),theta_y * (180 / PI),theta_z * (180 / PI)};
    cv::invert(rotM,invrotM);

    P = -invrotM * Tvec;
    //translation = {P.at<double>(0),P.at<double>(1),P.at<double>(2)};
    translation = {Tvec.at<double>(0),Tvec.at<double>(1),Tvec.at<double>(2)};
    //std::cout << P.size() << std::endl;
    if(config.show_pnp_axies){
        std::vector<cv::Point3f> refer_world_points = {cv::Point3f(0,0,0),cv::Point3f(translation.z / 10,0,0),cv::Point3f(0,translation.z / 10,0),cv::Point3f(0,0,translation.z / 10)};
        std::vector<cv::Point2f> refer_image_points;
        cv::projectPoints(refer_world_points,Rvec,Tvec,config.camConfig.mtx,config.camConfig.dist,refer_image_points);
        cv::Mat pnp = src.clone();
        cv::line(pnp,refer_image_points[0],refer_image_points[1],cv::Scalar(0,0,255),2);
        cv::line(pnp,refer_image_points[0],refer_image_points[2],cv::Scalar(0,255,0),2);
        cv::line(pnp,refer_image_points[0],refer_image_points[3],cv::Scalar(255,0,0),2);
        cv::imshow("pnp",pnp);
        cv::waitKey(1);
    }
}