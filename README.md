# 银河麒麟软件下载安装系统

为无网络连接的银河麒麟系统打包和安装软件的完整解决方案。

## 项目组成

本项目包含两个应用：

### 1. 安卓下载端 (kylin-software-downloader)
- **框架** - React Native + Expo SDK 54
- **功能** - 下载源管理、软件清单、下载设置、打包与下载
- **特点** - 支持多源、自动依赖分析、本地持久化

### 2. 银河麒麟安装端 (kylin-software-installer)
- **框架** - Qt 5 + C++17
- **功能** - 软件包解析、依赖分析、自动安装、进度追踪
- **特点** - 支持 APT/YUM/DNF、循环依赖检测、详细日志

## 快速开始

### 安卓下载端

```bash
cd kylin-software-downloader
npm install
npm run dev
```

### 银河麒麟安装端

```bash
cd kylin-software-installer
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## 文档

- **INTEGRATION_GUIDE.md** - 完整的集成工作流和使用指南
- **kylin-software-downloader/README.md** - 安卓端说明
- **kylin-software-installer/README.md** - 安装端说明
- **kylin-software-installer/ARCHITECTURE.md** - 详细的架构设计

## 系统要求

### 安卓下载端
- Android 8.0 或更高版本
- 2GB 以上内存

### 银河麒麟安装端
- 银河麒麟 V10/V20 ARM64
- Qt 5.12 或更高版本
- GCC 7.0 或更高版本

## 许可证

MIT License

## 联系方式

如有问题或建议，请提交 Issue。
