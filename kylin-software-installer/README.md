# 银河麒麟软件安装助手

**Kylin Software Installer** - 为银河麒麟 ARM64 系统设计的离线软件安装工具

## 项目概述

银河麒麟软件安装助手是一个专为银河麒麟操作系统设计的图形化软件安装工具。它与安卓下载端应用配合使用，用于在无网络连接的环境中安装软件包。

### 主要功能

- **软件包解析** - 支持 tar.gz 和 zip 格式的软件包
- **依赖分析** - 自动分析和检查软件依赖关系
- **智能安装** - 按照正确的依赖顺序自动安装软件
- **包管理器支持** - 支持 APT、YUM、DNF 等多种包管理器
- **详细日志** - 完整的安装过程日志记录
- **用户友好** - 简洁直观的图形化用户界面

## 系统要求

- **操作系统** - 银河麒麟 V10/V20 ARM64
- **Qt 版本** - Qt 5.12 或更高版本
- **编译器** - GCC 7.0 或更高版本
- **依赖库** - zlib, openssl, libarchive

## 构建和安装

### 前置条件

安装必要的开发工具和库：

```bash
# 对于 Debian/Ubuntu 系统
sudo apt-get install build-essential cmake qt5-qmake qt5-default \
    libqt5core5a libqt5gui5 libqt5widgets5 \
    zlib1g-dev libssl-dev libarchive-dev

# 对于 RedHat/CentOS 系统
sudo yum install gcc-c++ cmake qt5-qtbase-devel qt5-qtbase \
    zlib-devel openssl-devel libarchive-devel
```

### 编译

```bash
# 创建构建目录
mkdir build
cd build

# 运行 CMake
cmake ..

# 编译
make -j$(nproc)

# 安装（可选）
sudo make install
```

### 运行

```bash
# 直接运行
./kylin-software-installer

# 或从安装位置运行
/usr/local/bin/kylin-software-installer
```

## 使用说明

### 基本流程

1. **启动应用** - 打开银河麒麟软件安装助手
2. **选择软件包** - 选择从安卓下载端打包的软件包文件
3. **查看信息** - 查看软件包包含的软件列表和系统信息
4. **检查依赖** - 应用自动分析依赖关系并显示安装顺序
5. **开始安装** - 确认后自动安装所有软件及其依赖
6. **查看结果** - 查看安装完成状态和详细日志

### 软件包格式

软件包应该是由安卓下载端创建的压缩文件，包含以下结构：

```
kylin-packages-2024-01-26-120000.tar.gz
├── metadata.json          # 包元数据
├── packages/              # 软件包文件
│   ├── curl_7.68.0_arm64.deb
│   ├── git_2.34.1_arm64.deb
│   └── ...
└── dependencies.json      # 依赖关系表
```

#### metadata.json 格式

```json
{
  "version": "1.0",
  "timestamp": "2024-01-26T12:00:00Z",
  "targetSystem": "kylin",
  "targetArchitecture": "arm64",
  "packages": [
    {
      "id": "curl",
      "name": "curl",
      "version": "7.68.0",
      "size": 524288,
      "filename": "curl_7.68.0_arm64.deb",
      "checksum": "sha256:abc123..."
    }
  ],
  "totalSize": 52428800
}
```

#### dependencies.json 格式

```json
{
  "dependencies": {
    "curl": ["libcurl4", "libc6"],
    "git": ["perl", "openssh-client", "libc6"],
    "libcurl4": ["libc6"],
    "libc6": []
  }
}
```

## 项目结构

```
kylin-software-installer/
├── src/
│   ├── main.cpp                    # 应用入口
│   ├── mainwindow.h/cpp            # 主窗口
│   ├── welcomescreen.h/cpp         # 欢迎屏幕
│   ├── packageinfoscreen.h/cpp     # 软件包信息屏幕
│   ├── dependencyscreen.h/cpp      # 依赖检查屏幕
│   ├── installscreen.h/cpp         # 安装进度屏幕
│   ├── completescreen.h/cpp        # 完成屏幕
│   ├── packageparser.h/cpp         # 软件包解析器
│   ├── dependencyanalyzer.h/cpp    # 依赖分析器
│   ├── packagemanager.h/cpp        # 包管理器接口
│   └── logger.h/cpp                # 日志系统
├── resources/
│   ├── icons/                      # 应用图标
│   ├── styles/                     # 样式表
│   └── translations/               # 翻译文件
├── CMakeLists.txt                  # CMake 构建配置
└── README.md                       # 项目说明
```

## 核心模块说明

### PackageParser (软件包解析器)

负责解析和提取软件包文件，支持 tar.gz 和 zip 格式。

**主要功能：**
- 解析 metadata.json 获取包信息
- 解析 dependencies.json 获取依赖关系
- 提取压缩包到指定目录
- 验证包完整性

### DependencyAnalyzer (依赖分析器)

分析软件包之间的依赖关系，确定最优安装顺序。

**主要功能：**
- 构建依赖树
- 检测循环依赖
- 拓扑排序获取安装顺序
- 检查系统中已安装的包

### PackageManager (包管理器)

与系统包管理器交互，执行软件安装、卸载等操作。

**支持的包管理器：**
- APT (Debian/Ubuntu)
- YUM (RedHat/CentOS)
- DNF (Fedora)

**主要功能：**
- 自动检测系统包管理器
- 安装单个或多个软件包
- 检查软件包安装状态
- 卸载软件包

### Logger (日志系统)

记录应用运行过程中的所有信息，便于调试和问题排查。

**日志级别：**
- DEBUG - 调试信息
- INFO - 一般信息
- WARNING - 警告信息
- ERROR - 错误信息

## 故障排除

### 问题：无法检测到包管理器

**解决方案：**
- 检查系统是否安装了 apt、yum 或 dnf
- 确保这些工具在系统 PATH 中

### 问题：权限不足

**解决方案：**
- 应用需要 root 权限才能安装软件
- 使用 sudo 运行应用，或配置 sudoers 允许无密码执行

### 问题：软件包格式错误

**解决方案：**
- 确保软件包是由安卓下载端创建的
- 检查 metadata.json 和 dependencies.json 文件是否存在
- 验证 JSON 文件格式是否正确

## 开发指南

### 添加新的包管理器支持

在 `PackageManager` 类中添加新的包管理器类型：

```cpp
// 在 packagemanager.h 中
enum class PackageManagerType {
    APT,
    YUM,
    DNF,
    PACMAN,  // 新增
    Unknown
};

// 在 packagemanager.cpp 中的 detectPackageManager() 中添加
process.start("which", QStringList() << "pacman");
if (process.waitForFinished() && process.exitCode() == 0) {
    logger->info("检测到 PACMAN 包管理器");
    return PackageManagerType::PACMAN;
}
```

### 自定义用户界面

所有 UI 屏幕都继承自 `QWidget`，可以通过修改样式表来自定义外观：

```cpp
// 在各个屏幕的 initializeUI() 中修改 setStyleSheet()
button->setStyleSheet(
    "QPushButton {"
    "  background-color: #YOUR_COLOR;"
    "  ..."
    "}"
);
```

## 许可证

本项目采用 MIT 许可证。详见 LICENSE 文件。

## 贡献

欢迎提交 Issue 和 Pull Request 来改进本项目。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 GitHub Issue
- 发送邮件至 support@example.com

## 更新日志

### v1.0.0 (2024-01-26)

- 初始版本发布
- 支持基本的软件包安装功能
- 支持依赖分析和自动安装
- 支持 APT、YUM、DNF 包管理器
- 提供图形化用户界面和详细日志

