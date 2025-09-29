# USBApp

USBApp 是一个基于 WinUSB 的 Windows 控制台应用程序，用于与 USB 设备进行通信和测试。项目包含设备发现、管道读写、等时传输等功能，适合 USB 驱动开发和设备调试。

## 项目结构

- `device.cpp` / `device.h`  
  设备发现与 WinUSB 初始化相关代码，定义设备接口 GUID。
- `main.cpp`  
  控制台主程序，演示如何通过 WinUSB 与 USB 设备通信，包括 Bulk、Interrupt、Isochronous 管道的读写测试。
- `pch.h`  
  预编译头文件。
- `ReadMe.txt`  
  项目说明和 WinUSB 相关资料链接。
- `USBApp.vcxproj`  
  Visual Studio 项目文件。
- 其他：编译输出、日志、配置文件等。

## 主要功能

- 自动发现并连接 WinUSB 设备
- 支持 Bulk、Interrupt、Isochronous 管道数据传输
- 提供数据回环测试、速度测试等功能
- 支持多种 Windows 平台和架构（Win7/Win8/Win8.1，x86/x64）

## 构建方法

1. 安装 Visual Studio 2013 或更高版本，确保已安装 Windows Driver Kit (WDK)。
2. 打开 `USBApp.sln` 解决方案。
3. 选择合适的配置（如 Win7 Debug/x64），编译并运行。

## 使用方法

1. 安装 WinUSB 驱动（需根据设备硬件 ID 修改并安装 `USBApp.inf`）。
2. 运行编译后的程序，按提示选择测试用例和管道类型。
3. 查看控制台输出，进行 USB 通信测试。

## 参考资料

- [WinUSB 官方文档](https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/winusb)
- [WinUSB MSDN](http://msdn.microsoft.com/library/windows/hardware/ff540196.aspx)

## License

MIT License
