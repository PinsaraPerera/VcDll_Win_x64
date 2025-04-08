#ifndef VIRTUAL_CAMERA_SOURCE_H
#define VIRTUAL_CAMERA_SOURCE_H

#include <mfidl.h>
#include <atomic>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

class VirtualCameraSource : public IMFMediaSource {
public:
    VirtualCameraSource();
    virtual ~VirtualCameraSource();

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    // IMFMediaSource methods
    STDMETHODIMP GetCharacteristics(DWORD* pdwCharacteristics) override;
    STDMETHODIMP CreatePresentationDescriptor(IMFPresentationDescriptor** ppPresentationDescriptor) override;
    STDMETHODIMP Start(IMFPresentationDescriptor* pPresentationDescriptor, const GUID* pguidTimeFormat,
        const PROPVARIANT* pvarStartPosition) override;
    STDMETHODIMP Stop() override;
    STDMETHODIMP Pause() override;
    STDMETHODIMP Shutdown() override;

    // Custom methods
    HRESULT Initialize();
    HRESULT GetNextFrame(IMFSample** ppSample);

private:
    LONG m_refCount;
    std::mutex m_frameMutex;
    cv::VideoCapture m_capture;
    tesseract::TessBaseAPI m_tesseract;

    HRESULT ConvertMatToIMFSample(cv::Mat& frame, IMFSample** ppSample);
    bool ContainsSensitiveInfo(const cv::Mat& frame);
};

#endif // VIRTUAL_CAMERA_SOURCE_H