//#include "VirtualCameraSource.h" 
//#include <windows.h>
//#include <shlwapi.h>
//
//// {Your-GUID-Here} - Generate a new one in Visual Studio (Tools > Create GUID)
//const GUID CLSID_VirtualCamera =
//{ 0x1a2b3c4d, 0x5e6f, 0x7a8b, { 0x9c, 0x0d, 0x1e, 0x2f, 0x3a, 0x4b, 0x5c, 0x6d } };
//
//// Helper function to register the COM object in the Windows registry
//HRESULT RegisterServer()
//{
//    wchar_t modulePath[MAX_PATH];
//    if (!GetModuleFileNameW(nullptr, modulePath, MAX_PATH))
//        return HRESULT_FROM_WIN32(GetLastError());
//
//    HKEY hKey;
//    wchar_t clsidKey[MAX_PATH];
//    swprintf_s(clsidKey, L"Software\\Classes\\CLSID\\{%08lX-%04X-%04X-%04X-%04X%04X%04X}",
//        CLSID_VirtualCamera.Data1, CLSID_VirtualCamera.Data2, CLSID_VirtualCamera.Data3,
//        (CLSID_VirtualCamera.Data4[0] << 8) | CLSID_VirtualCamera.Data4[1],
//        (CLSID_VirtualCamera.Data4[2] << 8) | CLSID_VirtualCamera.Data4[3],
//        (CLSID_VirtualCamera.Data4[4] << 8) | CLSID_VirtualCamera.Data4[5],
//        (CLSID_VirtualCamera.Data4[6] << 8) | CLSID_VirtualCamera.Data4[7]);
//
//    if (RegCreateKeyExW(HKEY_CURRENT_USER, clsidKey, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
//        return E_FAIL;
//
//    RegSetValueExW(hKey, nullptr, 0, REG_SZ, (BYTE*)L"Virtual Camera", sizeof(L"Virtual Camera"));
//    RegSetValueExW(hKey, L"InprocServer32", 0, REG_SZ, (BYTE*)modulePath, (DWORD)(wcslen(modulePath) + 1) * sizeof(wchar_t));
//    RegCloseKey(hKey);
//
//    return S_OK;
//}
//
//// Helper function to unregister the COM object from the Windows registry
//HRESULT UnregisterServer()
//{
//    wchar_t clsidKey[MAX_PATH];
//    swprintf_s(clsidKey, L"Software\\Classes\\CLSID\\{%08lX-%04X-%04X-%04X-%04X%04X%04X}",
//        CLSID_VirtualCamera.Data1, CLSID_VirtualCamera.Data2, CLSID_VirtualCamera.Data3,
//        (CLSID_VirtualCamera.Data4[0] << 8) | CLSID_VirtualCamera.Data4[1],
//        (CLSID_VirtualCamera.Data4[2] << 8) | CLSID_VirtualCamera.Data4[3],
//        (CLSID_VirtualCamera.Data4[4] << 8) | CLSID_VirtualCamera.Data4[5],
//        (CLSID_VirtualCamera.Data4[6] << 8) | CLSID_VirtualCamera.Data4[7]);
//
//    if (RegDeleteTreeW(HKEY_CURRENT_USER, clsidKey) != ERROR_SUCCESS)
//        return E_FAIL;
//
//    return S_OK;
//}
//
//STDAPI DllRegisterServer()
//{
//    return RegisterServer();
//}
//
//STDAPI DllUnregisterServer()
//{
//    return UnregisterServer();
//}
//
//STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
//{
//    if (rclsid != CLSID_VirtualCamera)
//        return CLASS_E_CLASSNOTAVAILABLE;
//
//    VirtualCameraSource* pSource = new VirtualCameraSource();
//    HRESULT hr = pSource->QueryInterface(riid, ppv);
//    if (FAILED(hr))
//    {
//        delete pSource;
//        return hr;
//    }
//    return S_OK;
//}
//
//BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//        // Perform initialization if needed
//        break;
//    case DLL_PROCESS_DETACH:
//        // Perform cleanup if needed
//        break;
//    }
//    return TRUE;
//}
