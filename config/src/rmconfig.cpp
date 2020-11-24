#include <rmconfig.h>

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

    // data
    Json::Value data = root["config_data"];
    ARMOR_CAMERA_GAIN = data["ARMOR_CAMERA_GAIN"].asInt();
    ARMOR_CAMERA_EXPOSURE = data["ARMOR_CAMERA_EXPOSURE"].asInt();
    ENERGY_CAMERA_GAIN = data["ENERGY_CAMERA_GAIN"].asInt();
    ENERGY_CAMERA_EXPOSURE = data["ENERGY_CAMERA_EXPOSURE"].asInt();

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

    // data
    Json::Value data;
    data["ARMOR_CAMERA_GAIN"] = ARMOR_CAMERA_GAIN;
    data["ARMOR_CAMERA_EXPOSURE"] = ARMOR_CAMERA_EXPOSURE;
    data["ENERGY_CAMERA_GAIN"] = ENERGY_CAMERA_GAIN;
    data["ENERGY_CAMERA_EXPOSURE"] = ENERGY_CAMERA_EXPOSURE;

    root["config"] = config;
    root["config_data"] = data;
    std::ofstream ofs;
    ofs.open(configPath);
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &ofs);
    ofs.close();
}