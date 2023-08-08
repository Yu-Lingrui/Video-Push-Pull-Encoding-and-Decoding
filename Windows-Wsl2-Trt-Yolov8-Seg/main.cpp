#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    // Ubuntu安装ffmpeg：sudo apt-get install ffmpeg
    // rtsp地址，模拟四路视频流进行展示
    std::string rtsp1 = "rtsp://192.168.1.109/test";
    std::string rtsp2 = rtsp1;
    std::string rtsp3 = rtsp1;
    std::string rtsp4 = rtsp1;

    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream1 = cv::VideoCapture(rtsp1, cv::CAP_FFMPEG);
    cv::VideoCapture stream2 = cv::VideoCapture(rtsp2, cv::CAP_FFMPEG);
    cv::VideoCapture stream3 = cv::VideoCapture(rtsp3, cv::CAP_FFMPEG);
    cv::VideoCapture stream4 = cv::VideoCapture(rtsp4, cv::CAP_FFMPEG);

    if (!stream1.isOpened() || !stream2.isOpened() || !stream3.isOpened() || !stream4.isOpened())
    {
        std::cout << "有视频流未打开" << std::endl;
        return -1;
    }

    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat frame3;
    cv::Mat frame4;

    cv::Mat H1, H2, V, blur;

    // 使用namedWindow创建窗口，WINDOW_AUTOSIZE：自动调整窗口大小
    cv::namedWindow("rtsp_demo", cv::WINDOW_AUTOSIZE);

    while (true)
    {
        if (!stream1.read(frame1) || !stream2.read(frame2) || !stream3.read(frame3) || !stream4.read(frame4))
        {
            std::cout << "有视频流未读取" << std::endl;
            continue;
        }
        // 缩放等处理
        cv::resize(frame1, frame1, cv::Size(500, 300));

        cv::resize(frame2, frame2, cv::Size(500, 300));
        cv::flip(frame2, frame2, 1);

        cv::resize(frame3, frame3, cv::Size(500, 300));
        cv::cvtColor(frame1, frame1, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame1, frame1, cv::COLOR_GRAY2BGR);

        cv::resize(frame4, frame4, cv::Size(500, 300));
        cv::putText(frame4, "RTSP demo", cv::Point(100, 100), cv::FONT_ITALIC, 1, cv::Scalar(0, 0, 255), 2);
        // 拼接
        cv::hconcat(frame1, frame2, H1);
        cv::hconcat(frame3, frame4, H2);
        cv::vconcat(H1, H2, V);

        // 高斯模糊一下
        cv::GaussianBlur(V, blur, cv::Size(25, 25), 0);

        cv::imshow("rtsp_demo", blur);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    return 0;
}
