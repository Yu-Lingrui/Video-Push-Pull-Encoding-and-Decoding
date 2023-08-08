// tensorRT include
#include <NvInfer.h>
#include <NvInferRuntime.h>

// cuda include
#include <cuda_runtime.h>

// system include
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "../src/TrtLib/common/ilogger.hpp"
#include "../src/TrtLib/common/json.hpp"
#include "../src/TrtLib/builder/trt_builder.hpp"
#include "../src/app_yolo/yolo.hpp"
#include "../src/loadConfig.hpp"
#include "common.hpp"

const std::string engine_file = "../workspace/model/yolov5s.engine";
const std::string onnx_file = "../workspace/model/yolov5s.onnx";

// 图像队列
std::queue<cv::Mat> img_queue;

class InferInstance
{
public:
    bool startup()
    {
        yoloIns = get_infer(Yolo::Type::V5);
        return yoloIns != nullptr;
    }

    bool inference(const cv::Mat &image_input, Yolo::BoxArray &boxarray)
    {

        if (yoloIns == nullptr)
        {
            INFOE("Not Initialize.");
            return false;
        }

        if (image_input.empty())
        {
            INFOE("Image is empty.");
            return false;
        }
        boxarray = yoloIns->commit(image_input).get();
        return true;
    }

private:
    std::shared_ptr<Yolo::Infer> get_infer(Yolo::Type type)
    {
        if (!iLogger::exists(engine_file))
        {
            TRT::compile(
                TRT::Mode::FP32,
                10,
                onnx_file,
                engine_file);
        }
        else
        {
            INFOW("%s has been created!", engine_file.c_str());
        }
        return Yolo::create_infer(engine_file, type, 0, 0.25, 0.45);
    }
    std::shared_ptr<Yolo::Infer> yoloIns;
};


int main(int argc, char const *argv[])
{
    std::string in_url = "rtsp://192.168.1.103/test";
    iLogger::set_log_level(iLogger::LogLevel::Info);
    // 初始化yolo
    std::shared_ptr<InferInstance> yolo;
    yolo.reset(new InferInstance());
    if (!yolo->startup())
    {
        yolo.reset();
        exit(1);
    }
    Yolo::BoxArray boxarray;
    INFO("opencv version: %s", CV_VERSION);

    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream = cv::VideoCapture(in_url, cv::CAP_FFMPEG);

    if (!stream.isOpened())
    {
        std::cout << "有视频流未打开" << std::endl;
        return -1;
    }

    cv::Mat image;
    cv::namedWindow("rtsp_detect", cv::WINDOW_AUTOSIZE);

    while (true)
    {
        if (!stream.read(image))
        {
            std::cout << "有视频流未读取" << std::endl;
            continue;
        }
        // 图像加入队列
        if (img_queue.size() < 1000) img_queue.push(image);
        INFOD("yolo: the size of image queue : %d ", img_queue.size());

        // tensorrt推理处理展示
        // 取出队首图像
        if (img_queue.empty()) continue;
        cv::Mat Image = img_queue.front();
        img_queue.pop();

        yolo->inference(Image, boxarray);
            for (auto &box : boxarray)
            {

                cv::rectangle(Image, cv::Point(box.left, box.top), cv::Point(box.right, box.bottom), colors[box.class_label], 3);

                auto name = cocolabels[box.class_label];
                auto caption = cv::format("%s %.2f", name, box.confidence);
                int text_width = cv::getTextSize(caption, 0, 1, 2, nullptr).width + 10;

                cv::rectangle(Image, cv::Point(box.left - 3, box.top - 33), cv::Point(box.left + text_width, box.top), colors[box.class_label], -1);
                cv::putText(Image, caption, cv::Point(box.left, box.top - 5), 0, 1, cv::Scalar::all(0), 2, 16);
            }
        INFOD("yolo Inference done!");

        cv::imshow("rtsp_detect", Image);
        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }
    return 0;
}
