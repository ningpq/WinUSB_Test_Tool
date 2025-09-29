//
// Define below GUIDs
//
#include <initguid.h>

//
// Device Interface GUID.
// Used by all WinUsb devices that this application talks to.
// Must match "DeviceInterfaceGUIDs" registry value specified in the INF file.
// 7d245a5b-5879-45fb-ab56-0384778d7966
//
//DEFINE_GUID(GUID_DEVINTERFACE_USBApp,
//    0x49feebbb, 0xec85, 0x47d9, 0xa5, 0x34, 0x6d, 0x51, 0x85, 0xa7, 0x5d, 0xf8);

DEFINE_GUID(GUID_DEVINTERFACE_USBApp,
    0x7d245a5b, 0x5879, 0x45fb, 0xab, 0x56, 0x03, 0x84, 0x77, 0x8d, 0x79, 0x66);

typedef struct _DEVICE_DATA {

    BOOL                    HandlesOpen;
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle;
    TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, *PDEVICE_DATA;

HRESULT
OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
    );

VOID
CloseDevice(
    _Inout_ PDEVICE_DATA DeviceData
    );

HANDLE NewDevAdd();
HANDLE NewDevAdd2();