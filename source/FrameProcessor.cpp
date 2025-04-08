#include "FrameProcessor.h" // Ensure this is included first  
#include <tesseract/baseapi.h>  
#include <opencv2/opencv.hpp>  
#include <memory>  
#include <mutex>  
#include <stdexcept>  
#include <string>  
#include <cstring>  

// Implementation of FrameProcessor methods  
bool FrameProcessor::ContainsSensitiveInfo(const cv::Mat& frame) {  
   static tesseract::TessBaseAPI tess;  
   static std::once_flag initFlag;  

   std::call_once(initFlag, []() {  
       if (tess.Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY) != 0) {  
           throw std::runtime_error("Tesseract init failed");  
       }  
       tess.SetPageSegMode(tesseract::PSM_AUTO);  
   });  

   cv::Mat gray;  
   cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);  

   //tess.SetImage(gray.data, gray.cols, gray.rows, 1, gray.step);
   tess.SetImage(gray.data, static_cast<int>(gray.cols), static_cast<int>(gray.rows), 1, static_cast<int>(gray.step));
   std::unique_ptr<char[]> text(tess.GetUTF8Text());  

   if (text) {  
       const std::string sensitiveKeywords[] = { "API_KEY", "PASSWORD", "SECRET" };  
       for (const auto& keyword : sensitiveKeywords) {  
           if (strstr(text.get(), keyword.c_str())) {  
               return true;  
           }  
       }  
   }  
   return false;  
}  

cv::Mat FrameProcessor::ProcessFrame(const cv::Mat& frame) {  
   cv::Mat result = frame.clone();  
   if (ContainsSensitiveInfo(frame)) {  
       cv::putText(result, "WARNING: Sensitive Content", cv::Point(50, 50),  
           cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);  
   }  
   return result;  
}

// Implementation of VirtualCameraSource methods
