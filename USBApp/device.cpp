#include "pch.h"
#include <malloc.h>
#include <SetupAPI.h>


DEFINE_GUID(GUID_DEVINTERFACE_SMIUSBDISPLAY, 0x28adfe0f, 0x3d1b, 0x4c2f, 0xa0, 0x61, 0x1e, 0x1e, 0x87, 0xab, 0xe2, 0xac);

DEFINE_GUID(GUID_DEVINTERFACE_SMIUSBDISPLAY2, 0x49feebbb, 0xec85, 0x47d9, 0xa5, 0x34, 0x6d, 0x51, 0x85, 0xa7, 0x5d, 0xf8);


HANDLE OpenOneDevice(_In_ HDEVINFO HardwareDeviceInfo, _In_ PSP_DEVICE_INTERFACE_DATA DeviceInfoData, _In_ STRSAFE_LPSTR devName);

HANDLE NewDevAdd()
{
    ULONG NumberDevices;
    HANDLE hOut = INVALID_HANDLE_VALUE;
    HDEVINFO                 hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA deviceInfoData;
    ULONG                    i;
    BOOLEAN                  done;
    PUSB_DEVICE_DESCRIPTOR   tempDevDesc;

    char DeviceName[MAX_PATH];

    bool NewDevAdd = false;

    tempDevDesc = NULL;
    NumberDevices = 0;

    //SMIFalconDeviceVector.clear();

    //
    // Open a handle to the plug and play dev node.
    // SetupDiGetClassDevs() returns a device information set that contains 
    // info on all installed devices of a specified class.
    //
    hardwareDeviceInfo =
        SetupDiGetClassDevs(&GUID_DEVINTERFACE_SMIUSBDISPLAY,
        NULL, // Define no enumerator (global)
        NULL, // Define no
        (DIGCF_PRESENT |           // Only Devices present
        DIGCF_DEVICEINTERFACE)); // Function class devices.

    if (hardwareDeviceInfo == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    //
    // Take a wild guess at the number of devices we have;
    // Be prepared to realloc and retry if there are more than we guessed
    //
    NumberDevices = 4;
    done = FALSE;
    deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    HANDLE hDev = nullptr;
    i = 0;
    while (!done)
    {
        Sleep(100);
        NumberDevices *= 2;

        for (; i < NumberDevices; i++)
        {
            // SetupDiEnumDeviceInterfaces() returns information about device 
            // interfaces exposed by one or more devices. Each call returns 
            // information about one interface; the routine can be called 
            // repeatedly to get information about several interfaces exposed 
            // by one or more devices.
            if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
                0, // We don't care about specific PDOs
                &GUID_DEVINTERFACE_SMIUSBDISPLAY,
                i,
                &deviceInfoData))
            {
                hDev = OpenOneDevice(hardwareDeviceInfo, &deviceInfoData, DeviceName);
                if (hDev != INVALID_HANDLE_VALUE)
                {
                    done = TRUE;
                    break;
                }
            }
            else
            {
                if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    done = TRUE;
                    break;
                }
            }
        }
    }

    // SetupDiDestroyDeviceInfoList() destroys a device information set
    // and frees all associated memory.

    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

    return hDev;
}

HANDLE NewDevAdd2()
{
    ULONG NumberDevices;
    HANDLE hOut = INVALID_HANDLE_VALUE;
    HDEVINFO                 hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA deviceInfoData;
    ULONG                    i;
    BOOLEAN                  done;
    PUSB_DEVICE_DESCRIPTOR   tempDevDesc;

    char DeviceName[MAX_PATH];

    bool NewDevAdd = false;

    tempDevDesc = NULL;
    NumberDevices = 0;

    //SMIFalconDeviceVector.clear();

    //
    // Open a handle to the plug and play dev node.
    // SetupDiGetClassDevs() returns a device information set that contains 
    // info on all installed devices of a specified class.
    //
    hardwareDeviceInfo =
        SetupDiGetClassDevs(&GUID_DEVINTERFACE_SMIUSBDISPLAY2,
            NULL, // Define no enumerator (global)
            NULL, // Define no
            (DIGCF_PRESENT |           // Only Devices present
                DIGCF_DEVICEINTERFACE)); // Function class devices.

    if (hardwareDeviceInfo == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    //
    // Take a wild guess at the number of devices we have;
    // Be prepared to realloc and retry if there are more than we guessed
    //
    NumberDevices = 4;
    done = FALSE;
    deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    HANDLE hDev = nullptr;
    i = 0;
    while (!done)
    {
        Sleep(100);
        NumberDevices *= 2;

        for (; i < NumberDevices; i++)
        {
            // SetupDiEnumDeviceInterfaces() returns information about device 
            // interfaces exposed by one or more devices. Each call returns 
            // information about one interface; the routine can be called 
            // repeatedly to get information about several interfaces exposed 
            // by one or more devices.
            if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
                0, // We don't care about specific PDOs
                &GUID_DEVINTERFACE_SMIUSBDISPLAY2,
                i,
                &deviceInfoData))
            {
                hDev = OpenOneDevice(hardwareDeviceInfo, &deviceInfoData, DeviceName);
                if (hDev != INVALID_HANDLE_VALUE)
                {
                    done = TRUE;
                    break;
                }
            }
            else
            {
                if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    done = TRUE;
                    break;
                }
            }
        }
    }

    // SetupDiDestroyDeviceInfoList() destroys a device information set
    // and frees all associated memory.

    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

    return hDev;
}



HANDLE OpenOneDevice(_In_ HDEVINFO HardwareDeviceInfo, _In_ PSP_DEVICE_INTERFACE_DATA DeviceInfoData, _In_ STRSAFE_LPSTR devName)
{
    PSP_DEVICE_INTERFACE_DETAIL_DATA     functionClassDeviceData = NULL;
    ULONG                                predictedLength = 0;
    ULONG                                requiredLength = 0;
    HANDLE                               hOut = INVALID_HANDLE_VALUE;
    BOOL                                 bResult;
    WINUSB_INTERFACE_HANDLE              WinusbHandle;
    //
    // allocate a function class device data structure to receive the
    // goods about this particular device.
    //
    SetupDiGetDeviceInterfaceDetail(
        HardwareDeviceInfo,
        DeviceInfoData,
        NULL, // probing so no output buffer yet
        0, // probing so output buffer length of zero
        &requiredLength,
        NULL); // not interested in the specific dev-node


    predictedLength = requiredLength;
    // sizeof (SP_FNCLASS_DEVICE_DATA) + 512;

    functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(predictedLength);
    if (NULL == functionClassDeviceData)
    {
        return INVALID_HANDLE_VALUE;
    }
    functionClassDeviceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    //
    // Retrieve the information from Plug and Play.
    //
    if (!SetupDiGetDeviceInterfaceDetail(
        HardwareDeviceInfo,
        DeviceInfoData,
        functionClassDeviceData,
        predictedLength,
        &requiredLength,
        NULL))
    {
        free(functionClassDeviceData);
        return INVALID_HANDLE_VALUE;
    }

    (void)StringCchCopy(devName, 256, functionClassDeviceData->DevicePath);

    hOut = CreateFile(
        functionClassDeviceData->DevicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, // no SECURITY_ATTRIBUTES structure
        OPEN_EXISTING, // No special create flags
        FILE_FLAG_OVERLAPPED, // No special attributes
        NULL); // No template file

    DWORD eerr = GetLastError();

    if (INVALID_HANDLE_VALUE == hOut) {
        return INVALID_HANDLE_VALUE;
    }

    bResult = WinUsb_Initialize(hOut, &WinusbHandle);

    if (FALSE == bResult) {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(hOut);
        return INVALID_HANDLE_VALUE;
    }

    free(functionClassDeviceData);
    return WinusbHandle;
}




HRESULT
RetrieveDevicePath(
    _Out_bytecap_(BufLen) LPTSTR DevicePath,
    _In_                  ULONG  BufLen,
    _Out_opt_             PBOOL  FailureDeviceNotFound
    );

HRESULT
OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
    )
/*++

Routine description:

    Open all needed handles to interact with the device.

    If the device has multiple USB interfaces, this function grants access to
    only the first interface.

    If multiple devices have the same device interface GUID, there is no
    guarantee of which one will be returned.

Arguments:

    DeviceData - Struct filled in by this function. The caller should use the
        WinusbHandle to interact with the device, and must pass the struct to
        CloseDevice when finished.

    FailureDeviceNotFound - TRUE when failure is returned due to no devices
        found with the correct device interface (device not connected, driver
        not installed, or device is disabled in Device Manager); FALSE
        otherwise.

Return value:

    HRESULT

--*/
{
    HRESULT hr = S_OK;
    BOOL    bResult;

    DeviceData->HandlesOpen = FALSE;

    hr = RetrieveDevicePath(DeviceData->DevicePath,
                            sizeof(DeviceData->DevicePath),
                            FailureDeviceNotFound);

    if (FAILED(hr)) {

        return hr;
    }

    DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
                                          GENERIC_WRITE | GENERIC_READ,
                                          FILE_SHARE_WRITE | FILE_SHARE_READ,
                                          NULL,
                                          OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                          NULL);

    if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    bResult = WinUsb_Initialize(DeviceData->DeviceHandle,
                                &DeviceData->WinusbHandle);

    if (FALSE == bResult) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(DeviceData->DeviceHandle);
        return hr;
    }

    DeviceData->HandlesOpen = TRUE;
    return hr;
}

VOID
CloseDevice(
    _Inout_ PDEVICE_DATA DeviceData
    )
/*++

Routine description:

    Perform required cleanup when the device is no longer needed.

    If OpenDevice failed, do nothing.

Arguments:

    DeviceData - Struct filled in by OpenDevice

Return value:

    None

--*/
{
    if (FALSE == DeviceData->HandlesOpen) {

        //
        // Called on an uninitialized DeviceData
        //
        return;
    }

    WinUsb_Free(DeviceData->WinusbHandle);
    CloseHandle(DeviceData->DeviceHandle);
    DeviceData->HandlesOpen = FALSE;

    return;
}

HRESULT
RetrieveDevicePath(
    _Out_bytecap_(BufLen) LPTSTR DevicePath,
    _In_                  ULONG  BufLen,
    _Out_opt_             PBOOL  FailureDeviceNotFound
    )
/*++

Routine description:

    Retrieve the device path that can be used to open the WinUSB-based device.

    If multiple devices have the same device interface GUID, there is no
    guarantee of which one will be returned.

Arguments:

    DevicePath - On successful return, the path of the device (use with CreateFile).

    BufLen - The size of DevicePath's buffer, in bytes

    FailureDeviceNotFound - TRUE when failure is returned due to no devices
        found with the correct device interface (device not connected, driver
        not installed, or device is disabled in Device Manager); FALSE
        otherwise.

Return value:

    HRESULT

--*/
{
    BOOL                             bResult = FALSE;
    HDEVINFO                         deviceInfo;
    SP_DEVICE_INTERFACE_DATA         interfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
    ULONG                            length;
    ULONG                            requiredLength=0;
    HRESULT                          hr;

    if (NULL != FailureDeviceNotFound) {

        *FailureDeviceNotFound = FALSE;
    }

    //
    // Enumerate all devices exposing the interface
    //
    deviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USBApp,
                                     NULL,
                                     NULL,
                                     DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (deviceInfo == INVALID_HANDLE_VALUE) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    //
    // Get the first interface (index 0) in the result set
    //
    bResult = SetupDiEnumDeviceInterfaces(deviceInfo,
                                          NULL,
                                          &GUID_DEVINTERFACE_USBApp,
                                          0,
                                          &interfaceData);

    if (FALSE == bResult) {

        //
        // We would see this error if no devices were found
        //
        if (ERROR_NO_MORE_ITEMS == GetLastError() &&
            NULL != FailureDeviceNotFound) {

            *FailureDeviceNotFound = TRUE;
        }

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Get the size of the path string
    // We expect to get a failure with insufficient buffer
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                              &interfaceData,
                                              NULL,
                                              0,
                                              &requiredLength,
                                              NULL);

    if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Allocate temporary space for SetupDi structure
    //
    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
        LocalAlloc(LMEM_FIXED, requiredLength);

    if (NULL == detailData)
    {
        hr = E_OUTOFMEMORY;
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    length = requiredLength;

    //
    // Get the interface's path string
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                              &interfaceData,
                                              detailData,
                                              length,
                                              &requiredLength,
                                              NULL);

    if(FALSE == bResult)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        LocalFree(detailData);
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Give path to the caller. SetupDiGetDeviceInterfaceDetail ensured
    // DevicePath is NULL-terminated.
    //
    hr = StringCbCopy(DevicePath,
                      BufLen,
                      detailData->DevicePath);

    LocalFree(detailData);
    SetupDiDestroyDeviceInfoList(deviceInfo);

    return hr;
}
