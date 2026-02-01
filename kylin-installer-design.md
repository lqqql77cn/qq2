# 银河麒麟软件安装助手 - 设计文档

## 项目概述

**应用名称：** 银河麒麟软件安装助手 (Kylin Software Installer)

**目标平台：** 银河麒麟 ARM64 系统

**主要功能：** 从安卓下载端获取打包的软件，进行解析、依赖检查和自动安装

---

## 技术架构

### 开发框架选择

| 框架 | 优势 | 劣势 | 推荐度 |
|------|------|------|--------|
| **Qt 5/6** | 跨平台、原生界面、性能好 | 学习曲线陡 | ⭐⭐⭐⭐⭐ |
| **Python + PyQt** | 快速开发、易维护 | 性能略低、依赖多 | ⭐⭐⭐⭐ |
| **CLI (Bash/Python)** | 轻量级、无依赖 | 用户体验差 | ⭐⭐⭐ |

**最终选择：Qt 5 (C++)**
- 原生银河麒麟系统集成度高
- 性能优异，适合系统工具
- 提供现代化 UI 和深色模式支持

---

## 核心功能设计

### 1. 软件包解析模块

**输入格式：** tar.gz 或 zip 压缩包

**包结构：**
```
kylin-packages-2024-01-26-120000.tar.gz
├── metadata.json          # 包元数据
├── packages/              # 软件包文件
│   ├── curl_7.68.0_arm64.deb
│   ├── git_2.34.1_arm64.deb
│   └── ...
└── dependencies.json      # 依赖关系表
```

**metadata.json 结构：**
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

### 2. 依赖分析模块

**功能：**
- 读取 dependencies.json 获取依赖关系
- 构建依赖树，检查循环依赖
- 验证系统中已安装的依赖
- 生成安装顺序列表

**依赖关系表格式：**
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

### 3. 安装管理模块

**安装流程：**
1. 验证软件包完整性 (checksum)
2. 检查磁盘空间
3. 按依赖顺序安装软件包
4. 处理安装错误和回滚
5. 验证安装结果

**支持的包管理器：**
- APT (Debian/Ubuntu 系)
- YUM (RedHat/CentOS 系)
- DNF (Fedora 系)

---

## 用户界面设计

### 屏幕 1: 欢迎屏幕
- 应用标题和描述
- 选择软件包文件按钮
- 最近使用的软件包列表

### 屏幕 2: 软件包信息
- 显示软件包元数据
- 包含的软件列表
- 总大小和预计时间
- 开始安装按钮

### 屏幕 3: 依赖检查
- 显示依赖关系树
- 标记已安装/待安装的依赖
- 冲突检查结果
- 确认安装按钮

### 屏幕 4: 安装进度
- 总体进度条
- 当前安装的软件包名称
- 实时日志输出
- 暂停/取消按钮

### 屏幕 5: 安装完成
- 成功/失败状态
- 安装统计信息
- 错误日志 (如有)
- 打开应用/返回主页按钮

---

## 数据流设计

```
┌─────────────────────────────────────────┐
│  用户选择软件包文件                      │
└────────────┬────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────┐
│  解析 metadata.json 和 dependencies.json │
└────────────┬────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────┐
│  构建依赖树，检查系统中已安装的包        │
└────────────┬────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────┐
│  显示依赖检查结果，用户确认              │
└────────────┬────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────┐
│  按顺序安装软件包                        │
│  - 验证 checksum                        │
│  - 调用 apt/yum/dnf install             │
│  - 处理错误和回滚                       │
└────────────┬────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────┐
│  显示安装完成状态                        │
└─────────────────────────────────────────┘
```

---

## 关键技术点

### 1. 包管理器集成
```cpp
// 检测系统包管理器
QString detectPackageManager();

// 执行安装命令
bool installPackage(const QString& packagePath, 
                   const QString& packageManager);

// 检查包是否已安装
bool isPackageInstalled(const QString& packageName);
```

### 2. 依赖解析算法
```cpp
// 拓扑排序获取安装顺序
QStringList topologicalSort(const QMap<QString, QStringList>& dependencies);

// 检测循环依赖
bool hasCyclicDependency(const QMap<QString, QStringList>& dependencies);

// 递归获取所有依赖
QStringList getAllDependencies(const QString& package,
                               const QMap<QString, QStringList>& dependencies);
```

### 3. 错误处理和回滚
- 记录每个安装步骤
- 安装失败时自动回滚已安装的包
- 提供详细的错误日志
- 支持恢复中断的安装

---

## 文件结构

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
│   ├── packageparser.h/cpp         # 软件包解析
│   ├── dependencyanalyzer.h/cpp    # 依赖分析
│   ├── packagemanager.h/cpp        # 包管理器接口
│   └── logger.h/cpp                # 日志系统
├── resources/
│   ├── icons/
│   ├── styles/
│   └── translations/
├── CMakeLists.txt                  # 构建配置
└── README.md                       # 项目说明
```

---

## 系统要求

- **操作系统：** 银河麒麟 V10/V20 ARM64
- **Qt 版本：** Qt 5.12 或更高
- **编译器：** GCC 7.0 或更高
- **依赖库：** zlib, openssl, libarchive

---

## 开发计划

| 阶段 | 任务 | 预计时间 |
|------|------|---------|
| 1 | 项目初始化，UI 框架搭建 | 2 天 |
| 2 | 软件包解析和依赖分析 | 3 天 |
| 3 | 包管理器集成 | 2 天 |
| 4 | UI 屏幕实现 | 3 天 |
| 5 | 测试和优化 | 2 天 |
| 6 | 文档和部署 | 1 天 |

---

## 安全考虑

1. **软件包验证**
   - 使用 SHA256 校验和验证包完整性
   - 支持数字签名验证 (可选)

2. **权限管理**
   - 需要 root 权限进行安装
   - 使用 sudo 或 PolicyKit 提升权限

3. **错误处理**
   - 不允许部分安装状态
   - 安装失败时完全回滚
   - 记录所有操作日志

4. **用户隐私**
   - 不收集用户数据
   - 本地处理所有操作
   - 清晰的权限提示

