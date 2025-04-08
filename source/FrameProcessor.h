#ifndef FRAME_PROCESSOR_H
#define FRAME_PROCESSOR_H

#include <opencv2/opencv.hpp>

class FrameProcessor {
public:
    static bool ContainsSensitiveInfo(const cv::Mat& frame);
    static cv::Mat ProcessFrame(const cv::Mat& frame);
};

#endif // FRAME_PROCESSOR_H
