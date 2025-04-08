#include "VirtualCameraSource.h"
#include <stdexcept> // For std::runtime_error
#include <mfapi.h>   // For Media Foundation functions like MFStartup, MFShutdown
#include <mfobjects.h> // For IMFMediaBuffer, IMFSample
#include <mferror.h> // For Media Foundation error codes
#include <opencv2/opencv.hpp> // For OpenCV functions and cv::Mat
#include <tesseract/baseapi.h> // For Tesseract OCR
#include <mutex> // For std::mutex and std::lock_guard
#include <memory> // For std::unique_ptr
#include <windows.h> // For InterlockedIncrement and InterlockedDecrement
#include <initguid.h> // Ensures GUIDs are instantiated
#include <mfidl.h>    // For IMFMediaSource and related interfaces


#ifndef MF_E_UNSUPPORTED_OPERATION
#define MF_E_UNSUPPORTED_OPERATION ((HRESULT)0xC00D3E80L) // Explicitly define if not already defined
#endif

VirtualCameraSource::VirtualCameraSource() : m_refCount(1) {
    // Initialize Media Foundation
    MFStartup(MF_VERSION);

    // Initialize Tesseract OCR (English language)
    if (m_tesseract.Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY) != 0) {
        throw std::runtime_error("Could not initialize Tesseract");
    }
    m_tesseract.SetPageSegMode(tesseract::PSM_AUTO);
}

VirtualCameraSource::~VirtualCameraSource() {
    if (m_capture.isOpened()) {
        m_capture.release();
    }
    m_tesseract.End();
    MFShutdown();
}

// IUnknown methods
STDMETHODIMP VirtualCameraSource::QueryInterface(REFIID riid, void** ppv) {
    if (!ppv) return E_POINTER;

    if (riid == IID_IUnknown || riid == IID_IMFMediaSource) {
        *ppv = static_cast<IMFMediaSource*>(this);
        AddRef();
        return S_OK;
    }

    *ppv = nullptr;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) VirtualCameraSource::AddRef() {
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) VirtualCameraSource::Release() {
    ULONG count = InterlockedDecrement(&m_refCount);
    if (count == 0) {
        delete this;
    }
    return count;
}

// IMFMediaSource methods
STDMETHODIMP VirtualCameraSource::GetCharacteristics(DWORD* pdwCharacteristics) {
    if (!pdwCharacteristics) return E_POINTER;
    *pdwCharacteristics = MFMEDIASOURCE_IS_LIVE;
    return S_OK;
}

STDMETHODIMP VirtualCameraSource::Start(IMFPresentationDescriptor* pPresentationDescriptor,
    const GUID* pguidTimeFormat,
    const PROPVARIANT* pvarStartPosition) {
    std::lock_guard<std::mutex> lock(m_frameMutex);

    if (!m_capture.isOpened()) {
        m_capture.open(0, cv::CAP_MSMF);
        if (!m_capture.isOpened()) return E_FAIL;
    }

    return S_OK;
}

// Implementation of CreatePresentationDescriptor
STDMETHODIMP VirtualCameraSource::CreatePresentationDescriptor(IMFPresentationDescriptor** ppPresentationDescriptor) {
    if (!ppPresentationDescriptor) return E_POINTER;

    // Create a dummy presentation descriptor (replace with actual implementation if needed)
    *ppPresentationDescriptor = nullptr;
    return MF_E_UNSUPPORTED_OPERATION; // Return unsupported for now
}

// Implementation of Stop
STDMETHODIMP VirtualCameraSource::Stop() {
    std::lock_guard<std::mutex> lock(m_frameMutex);

    if (m_capture.isOpened()) {
        m_capture.release();
    }

    return S_OK;
}

// Implementation of Pause
STDMETHODIMP VirtualCameraSource::Pause() {
    // Media Foundation does not require a specific implementation for Pause in this case
    return MF_E_UNSUPPORTED_OPERATION; // Return unsupported for now
}

// Implementation of Shutdown
STDMETHODIMP VirtualCameraSource::Shutdown() {
    std::lock_guard<std::mutex> lock(m_frameMutex);

    if (m_capture.isOpened()) {
        m_capture.release();
    }

    return S_OK;
}

// Implementation of Initialize
HRESULT VirtualCameraSource::Initialize() {
    // Perform any necessary initialization here
    // For example, initialize the video capture or other resources
    if (!m_capture.open(0, cv::CAP_MSMF)) {
        return E_FAIL; // Return failure if the capture device cannot be opened
    }

    return S_OK;
}


HRESULT VirtualCameraSource::GetNextFrame(IMFSample** ppSample) {
    if (!ppSample) return E_POINTER;

    std::lock_guard<std::mutex> lock(m_frameMutex);

    cv::Mat frame;
    if (!m_capture.read(frame) || frame.empty()) {
        return E_FAIL;
    }

    // Process sensitive information
    if (ContainsSensitiveInfo(frame)) {
        cv::putText(frame, "WARNING: Sensitive Content", cv::Point(50, 50),
            cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
    }

    return ConvertMatToIMFSample(frame, ppSample);
}

bool VirtualCameraSource::ContainsSensitiveInfo(const cv::Mat& frame) {
    cv::Mat gray;
    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    //m_tesseract.SetImage(gray.data, gray.cols, gray.rows, 1, gray.step);
    m_tesseract.SetImage(gray.data, static_cast<int>(gray.cols), static_cast<int>(gray.rows), 1, static_cast<int>(gray.step));
    std::unique_ptr<char[]> text(m_tesseract.GetUTF8Text());

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

HRESULT VirtualCameraSource::ConvertMatToIMFSample(cv::Mat& frame, IMFSample** ppSample) {
    HRESULT hr = S_OK;
    IMFMediaBuffer* pBuffer = nullptr;

    // Ensure the buffer size is within the range of DWORD
    const size_t bufferSize = frame.total() * frame.elemSize();
    if (bufferSize > static_cast<size_t>(std::numeric_limits<DWORD>::max())) {
        return E_FAIL; // Fail if the buffer size exceeds DWORD's maximum value
    }

    // Create sample
    if (FAILED(hr = MFCreateSample(ppSample))) return hr;
    if (FAILED(hr = MFCreateMemoryBuffer(static_cast<DWORD>(bufferSize), &pBuffer))) { // Explicit cast to DWORD
        (*ppSample)->Release();
        return hr;
    }

    // Copy frame data
    BYTE* pData = nullptr;
    if (SUCCEEDED(hr = pBuffer->Lock(&pData, nullptr, nullptr))) {
        memcpy(pData, frame.data, bufferSize);
        pBuffer->Unlock();
    }

    // Add buffer to sample
    if (SUCCEEDED(hr)) {
        hr = (*ppSample)->AddBuffer(pBuffer);
    }

    pBuffer->Release();
    return hr;
}


//HRESULT VirtualCameraSource::ConvertMatToIMFSample(cv::Mat& frame, IMFSample** ppSample) {
//    HRESULT hr = S_OK;
//    IMFMediaBuffer* pBuffer = nullptr;
//
//    const DWORD bufferSize = frame.total() * frame.elemSize();
//
//    // Create sample
//    if (FAILED(hr = MFCreateSample(ppSample))) return hr;
//    if (FAILED(hr = MFCreateMemoryBuffer(bufferSize, &pBuffer))) {
//        (*ppSample)->Release();
//        return hr;
//    }
//
//    // Copy frame data
//    BYTE* pData = nullptr;
//    if (SUCCEEDED(hr = pBuffer->Lock(&pData, nullptr, nullptr))) {
//        memcpy(pData, frame.data, bufferSize);
//        pBuffer->Unlock();
//    }
//
//    // Add buffer to sample
//    if (SUCCEEDED(hr)) {
//        hr = (*ppSample)->AddBuffer(pBuffer);
//    }
//
//    pBuffer->Release();
//    return hr;
//}

// Implement other IMFMediaSource methods as needed
