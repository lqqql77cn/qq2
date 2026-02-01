# 银河麒麟软件下载安装系统 - 集成指南

## 项目概述

本项目包含两个应用，用于在无网络连接的环境中为银河麒麟系统打包和安装软件：

1. **安卓下载端** (`kylin-software-downloader`) - React Native 移动应用
2. **银河麒麟安装端** (`kylin-software-installer`) - Qt 桌面应用

---

## 应用架构

### 整体流程

```
┌─────────────────────────────────────────────────────────────┐
│                    用户工作流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  1. 在安卓设备上运行下载端应用                              │
│     ├─ 配置下载源 (apt/yum/dnf 仓库)                       │
│     ├─ 选择要下载的软件包                                   │
│     ├─ 选择目标系统和架构 (默认银河麒麟 arm64)             │
│     └─ 自动分析依赖并下载                                   │
│                                                              │
│  2. 下载端打包软件                                           │
│     ├─ 生成 metadata.json (包信息)                         │
│     ├─ 生成 dependencies.json (依赖关系)                   │
│     ├─ 收集所有 .deb/.rpm 文件                             │
│     └─ 压缩为 tar.gz 或 zip                                │
│                                                              │
│  3. 将打包文件传输到银河麒麟系统                            │
│     ├─ 通过 USB 传输                                        │
│     ├─ 通过网络共享                                         │
│     └─ 通过其他方式                                         │
│                                                              │
│  4. 在银河麒麟系统上运行安装端应用                          │
│     ├─ 选择打包的软件文件                                   │
│     ├─ 查看软件包信息和依赖关系                             │
│     ├─ 自动检测系统包管理器                                 │
│     └─ 按依赖顺序自动安装所有软件                           │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 安卓下载端应用

### 项目位置
```
/home/ubuntu/kylin-software-downloader/
```

### 技术栈
- **框架** - React Native + Expo SDK 54
- **语言** - TypeScript
- **状态管理** - Context API + AsyncStorage
- **UI 框架** - NativeWind (Tailwind CSS)

### 核心功能
1. **下载源管理** - 支持多个下载源配置
2. **软件清单** - 搜索和选择软件包
3. **下载设置** - 选择目标系统和架构
4. **依赖分析** - 自动计算和下载依赖
5. **打包功能** - 生成可安装的软件包

### 运行方式
```bash
cd /home/ubuntu/kylin-software-downloader

# 开发模式
npm run dev

# 在 Expo Go 中预览
npm run qr

# 生成 APK
# 点击 Management UI 中的 Publish 按钮
```

### 打包格式

下载端生成的软件包结构：

```
kylin-packages-2024-01-26-120000.tar.gz
├── metadata.json          # 包元数据
├── packages/              # 软件包文件
│   ├── curl_7.68.0_arm64.deb
│   ├── git_2.34.1_arm64.deb
│   └── ...
└── dependencies.json      # 依赖关系表
```

**metadata.json 示例：**
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

**dependencies.json 示例：**
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

---

## 银河麒麟安装端应用

### 项目位置
```
/home/ubuntu/kylin-software-installer/
```

### 技术栈
- **框架** - Qt 5
- **语言** - C++17
- **构建系统** - CMake
- **包管理器支持** - APT, YUM, DNF

### 核心功能
1. **软件包解析** - 支持 tar.gz 和 zip 格式
2. **依赖分析** - 构建依赖树，检测循环依赖
3. **自动安装** - 按依赖顺序自动安装软件
4. **进度追踪** - 实时显示安装进度
5. **日志记录** - 详细的安装日志

### 编译步骤

```bash
cd /home/ubuntu/kylin-software-installer

# 创建构建目录
mkdir build && cd build

# 运行 CMake
cmake ..

# 编译
make -j$(nproc)

# 安装（可选）
sudo make install
```

### 运行方式

```bash
# 直接运行
./kylin-software-installer

# 或从安装位置运行
/usr/local/bin/kylin-software-installer
```

### 系统要求

- **操作系统** - 银河麒麟 V10/V20 ARM64
- **Qt 版本** - Qt 5.12 或更高
- **编译器** - GCC 7.0 或更高
- **依赖库** - zlib, openssl, libarchive

---

## 集成工作流

### 步骤 1: 在安卓设备上配置下载端

1. 启动安卓下载端应用
2. 进入"下载源管理"，添加软件源：
   - 源名称：Debian 官方源
   - 源 URL：http://deb.debian.org/debian
   - 源类型：APT
3. 进入"软件清单"，搜索并选择要下载的软件
4. 进入"下载设置"，确认：
   - 目标系统：银河麒麟
   - 目标架构：arm64
   - 保存目录：选择存储位置

### 步骤 2: 下载并打包软件

1. 进入"打包与下载"屏幕
2. 点击"开始下载"
3. 应用自动：
   - 分析依赖关系
   - 下载所有软件包及依赖
   - 生成 metadata.json 和 dependencies.json
   - 打包为 tar.gz 或 zip

### 步骤 3: 传输到银河麒麟系统

将打包文件通过以下方式传输：
- USB 存储设备
- 网络共享（SMB/NFS）
- 其他传输方式

### 步骤 4: 在银河麒麟系统上安装

1. 启动银河麒麟安装端应用
2. 点击"选择软件包文件"
3. 选择从安卓下载端传来的打包文件
4. 查看软件包信息和依赖关系
5. 点击"开始安装"
6. 应用自动：
   - 检测系统包管理器
   - 分析依赖顺序
   - 按顺序安装所有软件
   - 显示安装进度和日志

---

## 数据格式规范

### metadata.json 规范

| 字段 | 类型 | 说明 |
|------|------|------|
| version | string | 格式版本，当前为 "1.0" |
| timestamp | string | 打包时间戳，ISO 8601 格式 |
| targetSystem | string | 目标系统，如 "kylin" |
| targetArchitecture | string | 目标架构，如 "arm64" |
| packages | array | 软件包列表 |
| totalSize | number | 总大小（字节） |

### PackageInfo 规范

| 字段 | 类型 | 说明 |
|------|------|------|
| id | string | 包唯一标识 |
| name | string | 包名称 |
| version | string | 包版本 |
| size | number | 包大小（字节） |
| filename | string | 包文件名 |
| checksum | string | SHA256 校验和 |

### dependencies.json 规范

```json
{
  "dependencies": {
    "package_name": ["dependency1", "dependency2", ...],
    ...
  }
}
```

---

## 错误处理

### 常见问题

**问题 1: 下载源连接失败**
- 检查网络连接
- 验证源 URL 是否正确
- 尝试更换下载源

**问题 2: 依赖分析失败**
- 检查 dependencies.json 格式
- 验证是否存在循环依赖
- 查看应用日志获取详细信息

**问题 3: 安装失败**
- 检查磁盘空间是否充足
- 验证是否有 root 权限
- 查看安装日志获取错误信息

---

## 扩展和定制

### 添加新的包管理器支持

在 `PackageManager` 类中添加：

```cpp
// 在 detectPackageManager() 中
process.start("which", QStringList() << "pacman");
if (process.waitForFinished() && process.exitCode() == 0) {
    return PackageManagerType::PACMAN;
}

// 在 installPackages() 中
case PackageManagerType::PACMAN:
    command = "sudo";
    arguments << "pacman" << "-U" << packagePaths;
    break;
```

### 自定义 UI 主题

修改各屏幕的 `setStyleSheet()` 方法来自定义颜色和样式。

### 添加新功能

1. 创建新的模块类
2. 在 MainWindow 中集成
3. 通过信号/槽进行通信

---

## 测试建议

### 单元测试

- 测试软件包解析功能
- 测试依赖分析算法
- 测试包管理器命令执行

### 集成测试

- 测试完整的下载流程
- 测试完整的安装流程
- 测试错误处理机制

### 系统测试

- 在实际银河麒麟系统上测试
- 测试不同的软件包组合
- 测试网络中断和恢复

---

## 部署和发布

### 安卓应用部署

1. 在 Management UI 中点击 Publish 按钮
2. 系统生成 APK 安装包
3. 下载 APK 文件
4. 在 Android 设备上安装

### 桌面应用部署

1. 在银河麒麟系统上编译
2. 运行 `sudo make install` 安装
3. 应用将安装到 `/usr/local/bin/`
4. 可通过应用菜单启动

---

## 许可证

本项目采用 MIT 许可证。

---

## 联系和支持

如有问题或建议，请：
- 查看项目文档
- 检查应用日志
- 参考故障排除指南

