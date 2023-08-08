#include <iostream>
#include <fstream>
#include "TrtLib/common/json.hpp"

extern Json::Value base_conf;
extern Json::Value trt_conf;
extern Json::Value log_conf;
extern Json::Value yolo_conf;
extern Json::Value video_conf;

bool LoadConfig()
{
    // 读取JSON文件
    Json::Value root;
    std::ifstream config_file("../workspace/config.json");
    if (config_file.fail())
    {
        std::cerr << "Failed to open config file." << std::endl;
        return false;
    }
    config_file >> root;
    config_file.close();

    // 解析JSON config对象
    Json::Value base_conf = root["Base"];   // base config data
    Json::Value trt_conf = root["Trt"];     // tensorrt config data
    Json::Value log_conf = root["Log"];     // iLogger config data
    Json::Value yolo_conf = root["Yolo"];   // Yolo config data
    Json::Value video_conf = root["Video"]; // ffmpeg video push config data

    return true;
}
