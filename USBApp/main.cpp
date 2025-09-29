#include "pch.h"

#include <stdio.h>
#include <Windows.h>
#include <malloc.h>
#include <process.h>
#include <stdlib.h>
#include <Conio.h>
#include <vector>


HANDLE hDev;
std::vector<UCHAR> PipeVec;

ULONG TestUSBSpeedWithHeader(UCHAR pipeID, ULONG DataSize, PVOID pBuf)//bytes/s
{
    ULONG speed = 0;
    BOOL bNeedFree = FALSE;
    if (DataSize == 0 || pBuf == NULL)
    {
        DataSize = 10 * 1024 * 1024;
        pBuf = malloc(DataSize);
        RtlCopyMemory(pBuf, "smifalconsta", 12);
        bNeedFree = TRUE;
    }
    LARGE_INTEGER T1, T2, F;
    QueryPerformanceFrequency(&F);

    while (1)
    {
        DWORD nR;
        QueryPerformanceCounter(&T1);
        DataSize = (rand()*GetTickCount()) % (10 * 1024 * 1024);
        BOOL r = WinUsb_WritePipe(hDev, pipeID, (PUCHAR)pBuf, DataSize, &nR, NULL);
        if (r)
        {
            QueryPerformanceCounter(&T2);

            ULONG Time = (double)(T2.QuadPart - T1.QuadPart) * 1000 / (double)F.QuadPart;
            if (Time > 0)
            {
                speed = DataSize / Time / 1024;
                printf("Write EP: %d, Speed: %d\n", pipeID, speed);
            }
            else
            {
                printf("Write EP: %d, Speed is too fast\n", pipeID);
            }
        }
        else
        {
            printf("Write EP: %d, Fail!!!!\n", pipeID);
            return 1;
        }
        Sleep(1000);
    }
    return 0;
}

void  __cdecl ReadEPThread(LPVOID para)
{
    PVOID pBuf = malloc(1024 * 1024);
    ULONG DataSize = 1024 * 1024, nR;
    BYTE _Pipe_ = *(BYTE*)para;
    while (1)
    {
        BOOL r = WinUsb_ReadPipe(hDev, _Pipe_, (PUCHAR)pBuf, DataSize, &nR, NULL);
        if (!r)
        {
            printf("Continue Read EP error\n");
            OutputDebugString("SMI:*****Continue Read EP error\n");
            Sleep(100);
        }
        else
        {
            printf("Continue Read EP Success\n");
            OutputDebugString("SMI:Continue Read EP Success");
        }
    }
}

ULONG TestUSBSpeed(UCHAR pipeID,ULONG DataSize,PVOID pBuf)//bytes/s
{
    ULONG speed = 0;
    BOOL bNeedFree = FALSE;
    ULONG _dataS = DataSize;
    printf("Input Size you want to test(if size==0, will use randon size 10M for every time):");
    scanf("%d", &_dataS);
    if (DataSize == 0 || pBuf == NULL)
    {
        if (_dataS <= 0)
        {
            DataSize = 10 * 1024 * 1024;
        }
        else
        {
            DataSize = _dataS;
        }
        pBuf = malloc(DataSize);
        bNeedFree = TRUE;
    }
    LARGE_INTEGER T1, T2,F;
    QueryPerformanceFrequency(&F);
    CHAR pOutString[256];
    while (1)
    {
        DWORD nR;
        QueryPerformanceCounter(&T1);
        if (_dataS <= 0)
        {
            DataSize = (rand()*GetTickCount()) % (10 * 1024 * 1024);
        }
        else
        {
            DataSize = _dataS;
        }
        BOOL r = WinUsb_WritePipe(hDev, pipeID, (PUCHAR)pBuf, DataSize, &nR, NULL);
        if (r)
        {
            QueryPerformanceCounter(&T2);

            ULONG Time = (double)(T2.QuadPart - T1.QuadPart) * 1000 / (double)F.QuadPart;
            if (Time > 0)
            {
                speed = DataSize / Time / 1024;
                printf("Write EP: %d,Data Size is:%d, Speed: %d\n", pipeID, DataSize,speed);
                sprintf_s(pOutString, "SMI:Write EP: %d,Data Size is:%d, Speed: %d\n", pipeID, DataSize, speed);
                OutputDebugString(pOutString);
            }
            else
            {
                printf("Write EP: %d,Data Size is:%d, Speed is too fast\n", pipeID);
                sprintf_s(pOutString, "SMI:Write EP: %d,Data Size is:%d, Speed is too fast\n", pipeID);
                OutputDebugString(pOutString);
            }
        }
        else
        {
            printf("Write EP: %d, Fail!!!!\n", pipeID);
            sprintf_s(pOutString, "SMI:Write EP: %d, Fail!!!!\n", pipeID);
            OutputDebugString(pOutString);
            return 1;
        }
        Sleep(5);
    }
    return 0;
}

VOID WriteReadData()
{
    UINT _t = 0;
    UINT Size = 0;
    UINT MaxSize = 15 * 1024 * 1024;
    PUCHAR pData = NULL;
    PUCHAR pRead = NULL;
    ULONG nReturn;
    while (1)
    {
        Size = (rand()*GetTickCount()) % MaxSize;
        if (Size % 512 == 0)
        {
            Size++;
        }
        pData = (PUCHAR)malloc(Size);
        if (pData == NULL)
        {
            printf("Malloc error\n");
            break;
        }
        for (int i = 0; i < Size; i++)
        {
            pData[i] = rand();
        }
        pRead = (PUCHAR)malloc(15*1024*1024);
        if (pRead == NULL)
        {
            printf("Malloc error\n");
            break;
        }
        printf("==Begin %dth, Size is%d==",_t,Size);
        if (WinUsb_WritePipe(hDev, 2, pData, Size, &nReturn, NULL))
        {
            printf("%d bytes has writen", nReturn);
            if (WinUsb_ReadPipe(hDev, 0x82, pRead, 15*1024*1024, &nReturn, NULL))
            {
                printf("%d bytes has read", nReturn);
                DWORD HasReturn = nReturn;
                while (HasReturn < Size)
                {
                    if (!WinUsb_ReadPipe(hDev, 0x82, pRead + HasReturn, Size - HasReturn, &nReturn, NULL))
                    {
                        printf("read error:%d\n", GetLastError());
                        break;
                    }
                    printf("****sub-read:%d bytes has read", nReturn);
                    HasReturn += nReturn;
                }
                if (memcmp(pRead, pData, Size) == 0)
                {
                    printf("==Read Success==\n");
                }
                else
                {
                    printf("#Error, data not match#\n");
                }
            }
            else
            {
                printf("read error:%d\n", GetLastError());
                break;
            }
        }
        else
        {
            printf("write error:%d\n",GetLastError());
            break;
        }
        free(pData);
        free(pRead);
        Sleep(1500);
        _t++;
    }
}

UCHAR WriteReadData(UCHAR pipeID)
{
    UINT _t = 0;
    UINT Size = 0;
    UINT MaxSize = 15 * 1024 * 1024;
    PUCHAR pData = NULL;
    PUCHAR pRead = NULL;
    ULONG nReturn = 0;


    Size = 400*1024;
    if (Size % 512 == 0)
    {
        Size++;
    }
    pData = (PUCHAR)malloc(Size);
    if (pData == NULL)
    {
        printf("Malloc error\n");
        return 1;
    }
    for (int i = 0; i < Size; i++)
    {
        pData[i] = rand();
    }
    pRead = (PUCHAR)malloc(15 * 1024 * 1024);
    if (pRead == NULL)
    {
        printf("Malloc error\n");
        return 2;
    }
    while (1)
    {
        printf("==Begin %dth, Size is%d==", _t, Size);
        if (WinUsb_WritePipe(hDev, pipeID, pData, Size, &nReturn, NULL))
        {
            printf("%d bytes has writen", nReturn);
            Sleep(20);
        }
        else
        {
            nReturn = GetLastError();
            printf("write error:%d\n", nReturn);
            break;
        }

        Sleep(20);
        _t++;
    }

    free(pData);
    free(pRead);

    return nReturn;
}

BOOL SendUSBData(UCHAR pipeID )//bytes/s
{
    ULONG speed = 0;
    BOOL bNeedFree = FALSE;
    ULONG ulDataSize = 0;
    PVOID pBuf = nullptr;
    LARGE_INTEGER T1, T2, F;
    QueryPerformanceFrequency(&F);

    printf("\nInput Size you want to send(input 0 exit):");
    scanf("%d", &ulDataSize);
    if (ulDataSize <= 0)
    {
        printf("Input Size is not valid.\n");
        return FALSE;
    }

    pBuf = malloc(ulDataSize);
    if (pBuf == nullptr)
    {
        printf("malloc buffer failed.\n");
        return FALSE;
    }

    while (1)
    {
        CHAR pOutString[256] = {0};

        DWORD nR;
        QueryPerformanceCounter(&T1);

        BOOL r = WinUsb_WritePipe(hDev, pipeID, (PUCHAR)pBuf, ulDataSize, &nR, NULL);
        if (r)
        {
            QueryPerformanceCounter(&T2);

            ULONG Time = (double)(T2.QuadPart - T1.QuadPart) * 1000 / (double)F.QuadPart;
            if (Time > 0)
            {
                speed = ulDataSize / Time / 1024;
                printf("Write EP: 0x%x, Data Size is:%d, Speed: %d\n", pipeID, ulDataSize, speed);
            }
            else
            {
                printf("Write EP: 0x%x, Data Size is:%d, Speed is too fast\n", pipeID);
            }
        }
        else
        {
            printf("Error: Write EP 0x%x Fail!!!!\n", pipeID);
            return 1;
        }

        int i = 5;
        while (i--)
        {
            if (kbhit())  //�Ƿ��м����£�ÿ�μ�ⲻ��ȴ����̶�����������״̬
            {
                char c = getch(); //ȡ�ô˴������ASCII��
                if (c == ' ')
                {
                    printf("End test\n");
                    if (pBuf)
                    {
                        free(pBuf);
                    }
                    return TRUE;
                }
            }
            Sleep(1);
        }
    }

    if (pBuf)
    {
        free(pBuf);
    }
    return TRUE;
}

BOOL USBDataLoopbackTest(UCHAR pipeID)
{
    ULONG speed = 0;
    BOOL bNeedFree = FALSE;
    ULONG ulDataSize = 0;
    PVOID pMallocBuf = nullptr;
    PVOID pData = nullptr;
    LARGE_INTEGER T1, T2, F;
    QueryPerformanceFrequency(&F);

    printf("\nInput Size you want to send(input 0 exit):");
    scanf("%d", &ulDataSize);
    if (ulDataSize <= 0)
    {
        printf("Input Size is not valid.\n");
        return FALSE;
    }

    pMallocBuf = malloc(ulDataSize);
    if (pMallocBuf == nullptr)
    {
        printf("malloc write buffer failed.\n");
        return FALSE;
    }
    pData = malloc(ulDataSize);
    if (pData == nullptr)
    {
        printf("malloc read buffer failed.\n");
        return FALSE;
    }

    while (1)
    {
        CHAR pOutString[256] = { 0 };
        CHAR Val = ++pOutString[0];
        //memset(pBuf, Val, ulDataSize);
        unsigned char tempdata = 0;
        unsigned char* pBuf = (unsigned char*)pMallocBuf;
        for (int i = 0; i < ulDataSize; i++)
        {
            pBuf[i] = tempdata++;
        }

        UCHAR _pipeID = 0;
        for (int k = 0; k < PipeVec.size(); k++)
        {
            if (pipeID == 0)
            {
                _pipeID = PipeVec[k];
            }
            else
            {
                _pipeID = pipeID;
            }

            DWORD nR;
            QueryPerformanceCounter(&T1);

            BOOL r = WinUsb_WritePipe(hDev, _pipeID, (PUCHAR)pBuf, ulDataSize, &nR, NULL);
            if (r)
            {
                QueryPerformanceCounter(&T2);

                ULONG Time = (double)(T2.QuadPart - T1.QuadPart) * 1000 / (double)F.QuadPart;
                if (Time > 0)
                {
                    speed = ulDataSize / Time / 1024;
                    printf("Write EP: 0x%x, Data Size is:%d, Speed: %d\n", _pipeID, ulDataSize, speed);
                }
                else
                {
                    printf("Write EP: 0x%x, Data Size is:%d, Speed is too fast\n", _pipeID, ulDataSize);
                }
            }
            else
            {
                printf("Error: Write EP %d Fail!!!!\n", _pipeID);
                return 1;
            }
 
            UINT32 DataSize = 0;
            UINT32 DataSizeOneTime = 0;
            UCHAR _PipeRead = _pipeID + 0x80;

            while (1)
            {
                r = WinUsb_ReadPipe(hDev, _PipeRead, (PUCHAR)pData+DataSize, ulDataSize, (PULONG)&DataSizeOneTime, NULL);
                if (r)
                {
                    DataSize += DataSizeOneTime;
                    if (DataSize != ulDataSize)
                    {
                        continue;
                    }
                    if (DataSize != ulDataSize)
                    {
                        printf("Error: Read data size not match(size %d), EP: 0x%x\n", DataSize, _PipeRead);
                    }
                    else
                    {
                        int ret = memcmp(pBuf, pData, DataSize);
                        if (ret)
                        {
                            printf("Error: Read data not match, EP: 0x%x\n", _PipeRead);
                        }
                        else
                        {
                            printf("Read EP: 0x%x Succeed. Data size %d\n", _PipeRead, DataSize);
                        }
                    }
                }
                else
                {
                    printf("Error: Read data error %d, EP: 0x%x\n", GetLastError(), _PipeRead);
                }
                break;
            }



            int i = 5;
            while (i--)
            {
                if (kbhit())  //�Ƿ��м����£�ÿ�μ�ⲻ��ȴ����̶�����������״̬
                {
                    char c = getch(); //ȡ�ô˴������ASCII��
                    if (c == ' ')
                    {
                        printf("End test\n");
                        if (pBuf)
                        {
                            free(pBuf);
                        }
                        if (pData)
                        {
                            free(pData);
                        }
                        return TRUE;
                    }
                }
                Sleep(1);
            }
        }

    }

    if (pMallocBuf)
    {
        free(pMallocBuf);
    }
    if (pData)
    {
        free(pData);
    }
    return TRUE;
}


BOOL USBIsochTest(UCHAR pipeID)
{
    ULONG speed = 0;
    BOOL bNeedFree = FALSE;
    ULONG ulDataSize = 0;
    PVOID pBuf = nullptr;
    PVOID pData = nullptr;
    LARGE_INTEGER T1, T2, F;
    QueryPerformanceFrequency(&F);

    printf("\nInput Size you want to send(input 0 exit):");
    scanf("%d", &ulDataSize);

    LPOVERLAPPED overlapped = new OVERLAPPED;
    ZeroMemory(overlapped, sizeof(OVERLAPPED));

    overlapped->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (overlapped->hEvent == NULL)
    {
        printf("Unable to set event for overlapped operation.\n");
        return FALSE;
    }

    ULONG IsochTimes = 10;
    ULONG BufferLength = ulDataSize * IsochTimes;
    PUCHAR pBuffer = (PUCHAR)malloc(BufferLength);
    WINUSB_ISOCH_BUFFER_HANDLE BufferHandle;
    BOOL Ret = WinUsb_RegisterIsochBuffer(hDev, pipeID, pBuffer, BufferLength, &BufferHandle);
    if (Ret != TRUE)
    {
        printf("Error: register Isoch buffer fail ErrorCode: 0x%x!\n", GetLastError());
        return FALSE;
    }


    CHAR Val = pBuffer[0]++;

    for (int k = 0; k < IsochTimes; k++)
    {
        memset( pBuffer + k * ulDataSize, Val, ulDataSize);
        DWORD nFrameNum = 0;
        QueryPerformanceCounter(&T1);

        BOOL r = WinUsb_WriteIsochPipe(BufferHandle, k * ulDataSize, ulDataSize, &nFrameNum, overlapped);
        if (r)
        {
            QueryPerformanceCounter(&T2);

            ULONG Time = (double)(T2.QuadPart - T1.QuadPart) * 1000 / (double)F.QuadPart;
            if (Time > 0)
            {
                speed = ulDataSize / Time / 1024;
                printf("Write Isoch buffer size:%d, Speed: %d\n", ulDataSize, speed);
            }
            else
            {
                printf("Write Isoch buffer size:%d, Speed is too fast\n", ulDataSize);
            }
        }
        else
        {
            printf("Error: write Isoch buffer fail ErrorCode: 0x%x!\n", GetLastError());
            return FALSE;
        }

        USBD_ISO_PACKET_DESCRIPTOR IsoPacketDescriptors;
        r = WinUsb_ReadIsochPipe(BufferHandle, k * ulDataSize, ulDataSize, &nFrameNum, 1, &IsoPacketDescriptors, nullptr);
        if (r)
        {
            printf("Read Isoch buffer size:%d\n", ulDataSize);
        }
        else
        {
            printf("Read Isoch buffer ErrorCode: 0x%x\n", GetLastError());
        }
    }
    return TRUE;
}


#define ISOCH_TRANSFER_COUNT   3

VOID
SendIsochOutTransfer(
    _Inout_ UCHAR pipeID,
    _In_ BOOL AsapTransfer
)
{
    PUCHAR writeBuffer;
    LPOVERLAPPED overlapped;
    ULONG numBytes;
    BOOL result;
    DWORD lastError;
    WINUSB_ISOCH_BUFFER_HANDLE isochWriteBufferHandle;
    ULONG frameNumber;
    ULONG startFrame;
    LARGE_INTEGER timeStamp;
    ULONG i;
    ULONG totalTransferSize;
    ULONG IsochOutTransferSize = 0;
    ULONG IsochOutBytesPerFrame = 0;

    isochWriteBufferHandle = INVALID_HANDLE_VALUE;
    writeBuffer = NULL;
    overlapped = NULL;

    printf(_T("\n\nWrite transfer.\n"));
    printf("\nInput Isoch Out Transfer Size(input 0 exit):");
    scanf("%d", &IsochOutTransferSize);
    printf("\nInput Isoch Out Bytes Per Frame(input 0 exit):");
    scanf("%d", &IsochOutBytesPerFrame);

    totalTransferSize = IsochOutTransferSize * ISOCH_TRANSFER_COUNT;

    if (totalTransferSize % IsochOutBytesPerFrame != 0)
    {
        printf(_T("Transfer size must end at a frame boundary.\n"));
        goto Error;
    }

    writeBuffer = new UCHAR[totalTransferSize];

    if (writeBuffer == NULL)
    {
        printf(_T("Unable to allocate memory.\n"));
        goto Error;
    }

    ZeroMemory(writeBuffer, totalTransferSize);

    overlapped = new OVERLAPPED[ISOCH_TRANSFER_COUNT];
    if (overlapped == NULL)
    {
        printf("Unable to allocate memory.\n");
        goto Error;

    }

    ZeroMemory(overlapped, (sizeof(OVERLAPPED) * ISOCH_TRANSFER_COUNT));

    for (i = 0; i < ISOCH_TRANSFER_COUNT; i++)
    {
        overlapped[i].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (overlapped[i].hEvent == NULL)
        {
            printf("Unable to set event for overlapped operation.\n");
            goto Error;

        }
    }

    result = WinUsb_RegisterIsochBuffer(
        hDev,
        pipeID,
        writeBuffer,
        totalTransferSize,
        &isochWriteBufferHandle);

    if (!result)
    {
        printf(_T("Isoch buffer registration failed.\n"));
        goto Error;
    }

    result = WinUsb_GetCurrentFrameNumber(
        hDev,
        &frameNumber,
        &timeStamp);

    if (!result)
    {
        printf(_T("WinUsb_GetCurrentFrameNumber failed.\n"));
        goto Error;
    }

    startFrame = frameNumber + 5;

    for (i = 0; i < ISOCH_TRANSFER_COUNT; i++)
    {

        if (AsapTransfer)
        {
            result = WinUsb_WriteIsochPipeAsap(
                isochWriteBufferHandle,
                IsochOutTransferSize * i,
                IsochOutTransferSize,
                (i == 0) ? FALSE : TRUE,
                &overlapped[i]);

            printf(_T("Write transfer sent by using ASAP flag.\n"));
        }
        else
        {

            printf("Transfer starting at frame %d.\n", startFrame);

            result = WinUsb_WriteIsochPipe(
                isochWriteBufferHandle,
                i * IsochOutTransferSize,
                IsochOutTransferSize,
                &startFrame,
                &overlapped[i]);

            printf("Next transfer frame %d.\n", startFrame);

        }

        if (!result)
        {
            lastError = GetLastError();

            if (lastError != ERROR_IO_PENDING)
            {
                printf("Failed to send write transfer with error %x\n", lastError);
            }
        }
    }

    for (i = 0; i < ISOCH_TRANSFER_COUNT; i++)
    {
        result = WinUsb_GetOverlappedResult(
            hDev,
            &overlapped[i],
            &numBytes,
            TRUE);

        if (!result)
        {
            lastError = GetLastError();

            printf("Write transfer %d with error %x\n", i, lastError);
        }
        else
        {
            printf("Write transfer %d completed. \n", i);

        }
    }


Error:
    if (isochWriteBufferHandle != INVALID_HANDLE_VALUE)
    {
        result = WinUsb_UnregisterIsochBuffer(isochWriteBufferHandle);
        if (!result)
        {
            printf(_T("Failed to unregister isoch write buffer. \n"));
        }
    }

    if (writeBuffer != NULL)
    {
        delete[] writeBuffer;
    }

    for (i = 0; i < ISOCH_TRANSFER_COUNT; i++)
    {
        if (overlapped[i].hEvent != NULL)
        {
            CloseHandle(overlapped[i].hEvent);
        }

    }

    if (overlapped != NULL)
    {
        delete[] overlapped;
    }


    return;
}

LONG __cdecl
_tmain(
    LONG     Argc,
    LPTSTR * Argv
    )
/*++

Routine description:

    Sample program that communicates with a USB device using WinUSB

--*/
{
    //DEVICE_DATA           deviceData;
    HRESULT               hr;
    USB_DEVICE_DESCRIPTOR deviceDesc;
    BOOL                  bResult;
    BOOL                  noDevice;
    ULONG                 lengthReceived;
    
    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);

    //
    // Find a device connected to the system that has WinUSB installed using our
    // INF
    //
    hDev = NewDevAdd();
    if (hDev == NULL)
    {
        hDev = NewDevAdd2();
        if (hDev == NULL)
        {
            printf("Fail: Can't find falconUSB\n");
            system("Pause");
            return 0;
        }
    }

    //
    // Get device descriptor
    //
    bResult = WinUsb_GetDescriptor(hDev,
                                   USB_DEVICE_DESCRIPTOR_TYPE,
                                   0,
                                   0,
                                   (PBYTE) &deviceDesc,
                                   sizeof(deviceDesc),
                                   &lengthReceived);
    
    USB_INTERFACE_DESCRIPTOR UsbInterface;
    for (UCHAR AltSettingNum = 0; AltSettingNum < 16; AltSettingNum++)
    {
        if (WinUsb_QueryInterfaceSettings(hDev, AltSettingNum, &UsbInterface))
        {
            if (UsbInterface.bNumEndpoints > 2)
            {
                WinUsb_SetCurrentAlternateSetting(hDev, UsbInterface.bAlternateSetting);
                break;
            }
        }
    }

    printf("Connect device is 0x%x\n", deviceDesc.idProduct);

    //
    // Test Menu:
    //
    UCHAR TestCase = 0;
    printf("\n#######Test Case Choose########\n");
    printf("Input the test case:\n");
    printf("   Test Exit:                           0\n");
    printf("   Test Single Pipe Write:              1\n");
    printf("   Test Pipe Data Loopback:             2\n");
    printf("   Test Isoch Pipe Write:               3\n");
    printf("   Test Bulk In/Out Simutaniously:      4\n");
    while (1)
    {
        scanf("%d", &TestCase);

        if (TestCase == 0)
        {
            printf("End test;\n");
            return TRUE;
        }
        else if (TestCase > 4)
        {
            printf("The test case is wrong. Please try again.\n");
            continue;
        }
        else
        {
            break;
        }

        Sleep(5);
    }

    std::vector<UCHAR> _PipeVec;
    std::vector<WINUSB_PIPE_INFORMATION> _PipeInfoVec;
    WINUSB_PIPE_INFORMATION PipeInfo;
    for (UCHAR i = 0; i < UsbInterface.bNumEndpoints; i++)
    {
        BOOL hr = WinUsb_QueryPipe(hDev, UsbInterface.bAlternateSetting, i, &PipeInfo);
        if (hr == FALSE)
            break;
        _PipeInfoVec.push_back(PipeInfo);
        _PipeVec.push_back(PipeInfo.PipeId);

        WinUsb_ResetPipe(hDev, PipeInfo.PipeId);
        Sleep(0);
    }


    while (1)
    {
        printf("\n#######Test Pipe Choose########\n");
        printf("Input the pipe you want to test:\n");
        printf("   Input 0 test all:\n");
        for (UCHAR i = 0; i < _PipeVec.size(); i++)
        {
            if ((_PipeVec[i] & 0xF0) == 0)
            {
                for (UCHAR j = 0; j < _PipeInfoVec.size(); j++)
                {
                    if (_PipeVec[i] == _PipeInfoVec[j].PipeId)
                    {
                        if (USB_ENDPOINT_DIRECTION_OUT(_PipeInfoVec[j].PipeId))
                        {
                            if (_PipeInfoVec[j].PipeType == UsbdPipeTypeBulk)
                            {
                                printf("   Input %d For Bulk:\n", _PipeInfoVec[j].PipeId);
                                PipeVec.push_back(_PipeInfoVec[j].PipeId);
                            }
                            else if (_PipeInfoVec[j].PipeType == UsbdPipeTypeInterrupt)
                            {
                                printf("   Input %d For Interrupt:\n", _PipeInfoVec[j].PipeId);
                                PipeVec.push_back(_PipeInfoVec[j].PipeId);
                            }
                            else if (_PipeInfoVec[j].PipeType == UsbdPipeTypeControl)
                            {
                                printf("   Input %d For Control:\n", _PipeInfoVec[j].PipeId);
                                PipeVec.push_back(_PipeInfoVec[j].PipeId);
                            }
                            else if (_PipeInfoVec[j].PipeType == UsbdPipeTypeIsochronous)
                            {
                                printf("   Input %d For Iso:\n", _PipeInfoVec[j].PipeId);
                                PipeVec.push_back(_PipeInfoVec[j].PipeId);
                            }
                            else
                            {

                            }
                        }
                    }
                }
            }
        }
        printf("   Input 255 exit:\n");


        UCHAR _Pipe_ = 0;
        scanf("%d", &_Pipe_);
        if (_Pipe_ == 255)
        {
            break;
        }
        if (_Pipe_ > PipeVec.size())
        {
            printf("The pipe id is wrong. Please try again.\n");
            continue;
        }

        //if (TestCase == 3)
        //{
        //    _beginthread(ReadEPThread, 0, NULL);
        //    TestUSBSpeed(_Pipe_, 0, NULL);
        //    WriteReadData();
        //}
        //else 
        if (TestCase == 1)
        {
            SendUSBData(_Pipe_);
        }
        else if (TestCase == 2)
        {
            USBDataLoopbackTest(_Pipe_);
        }
        else if (TestCase == 3)
        {
            //USBIsochTest(_Pipe_);
            SendIsochOutTransfer(_Pipe_, false);
        }
        else if(TestCase == 4)
        {
            _beginthread(ReadEPThread, _Pipe_, NULL);
            Sleep(300);
            WriteReadData(_Pipe_ | 0x80);
        }
    }
    
    //system("Pause");
    return 0;

}
