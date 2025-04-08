// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

// Windows SDK
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unknwn.h>

// Standard Library
#include <memory>
#include <atomic>
#include <string>
#include <mutex>
#include <vector>
#include <stdexcept>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

// Tesseract
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

// Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mferror.h>

#endif //PCH_H


