#include <rmconfig.h>

void CameraConfig::init() {
    mtx = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    dist = (cv::Mat_<double>(1, 5) << k1, k2, p1, p2, k3);
    cv::initUndistortRectifyMap(mtx, dist, cv::noArray(), mtx,
                                cv::Size(640, 480), CV_32FC1, mapx, mapy);
}

void RmConfig::init_from_file() {
    std::ifstream ifs;
    ifs.open(configPath);
    if (!ifs.is_open()) {
        LOG(WARNING) << "config file not found! use default values";
        return;
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::Value root;

    if (!Json::parseFromStream(readerBuilder, ifs, &root, &errs)) {
        LOG(WARNING) << "reader parse error: " << errs;
        LOG(WARNING) << "config file load error! use default values.";
        return;
    }
    // config
    Json::Value config = root["config"];
    show_origin = config["show_origin"].asBool();
    show_armor_box = config["show_armor_box"].asBool();
    show_light_box = config["show_light_bo"].asBool();
    show_class_id = config["show_class_id"].asBool();
    wait_uart = config["wait_uart"].asBool();
    save_video = config["save_video"].asBool();
    show_light_blobs = config["show_light_blobs"].asBool();
    save_labelled_boxes = config["save_labelled_boxes"].asBool();
    show_pnp_axies = config["show_pnp_axies"].asBool();

    // data
    Json::Value data = root["config_data"];
    ARMOR_CAMERA_GAIN = data["ARMOR_CAMERA_GAIN"].asInt();
    ARMOR_CAMERA_EXPOSURE = data["ARMOR_CAMERA_EXPOSURE"].asInt();
    ENERGY_CAMERA_GAIN = data["ENERGY_CAMERA_GAIN"].asInt();
    ENERGY_CAMERA_EXPOSURE = data["ENERGY_CAMERA_EXPOSURE"].asInt();
    ENEMY_COLOR = data["ENEMY_COLOR"].asInt();
    ANTI_TOP = data["ANTI_TOP"].asInt();

    // camera
    Json::Value camera = root["camera"];
    camConfig.fx = camera["fx"].asDouble();
    camConfig.fy = camera["fy"].asDouble();
    camConfig.cx = camera["cx"].asDouble();
    camConfig.cy = camera["cy"].asDouble();
    camConfig.k1 = camera["k1"].asDouble();
    camConfig.k2 = camera["k2"].asDouble();
    camConfig.p1 = camera["p1"].asDouble();
    camConfig.p2 = camera["p2"].asDouble();
    camConfig.k3 = camera["k3"].asDouble();
    camConfig.init();

    ifs.close();
}

void RmConfig::write_to_file() {
    Json::Value root;
    // config
    Json::Value config;
    config["show_origin"] = show_origin;
    config["show_armor_box"] = show_armor_box;
    config["show_light_box"] = show_light_box;
    config["show_class_id"] = show_class_id;
    config["wait_uart"] = wait_uart;
    config["save_video"] = save_video;
    config["show_light_blobs"] = show_light_blobs;
    config["save_labelled_boxes"] = save_labelled_boxes;
    config["show_pnp_axies"] = show_pnp_axies;

    // data
    Json::Value data;
    data["ARMOR_CAMERA_GAIN"] = ARMOR_CAMERA_GAIN;
    data["ARMOR_CAMERA_EXPOSURE"] = ARMOR_CAMERA_EXPOSURE;
    data["ENERGY_CAMERA_GAIN"] = ENERGY_CAMERA_GAIN;
    data["ENERGY_CAMERA_EXPOSURE"] = ENERGY_CAMERA_EXPOSURE;
    data["ENEMY_COLOR"] = ENEMY_COLOR;
    data["ANTI_TOP"] = ANTI_TOP;

    // camera
    Json::Value camera;
    camera["fx"] = camConfig.fx;
    camera["fy"] = camConfig.fy;
    camera["cx"] = camConfig.cx;
    camera["cy"] = camConfig.cy;
    camera["k1"] = camConfig.k1;
    camera["k2"] = camConfig.k2;
    camera["p1"] = camConfig.p1;
    camera["p2"] = camConfig.p2;
    camera["k3"] = camConfig.k3;

    root["config"] = config;
    root["config_data"] = data;
    root["camera"] = camera;

    std::ofstream ofs;
    ofs.open(configPath);
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &ofs);
    ofs.close();
}