# 银河麒麟软件安装助手 - 架构文档

## 系统架构概览

```
┌─────────────────────────────────────────────────────────┐
│         银河麒麟软件安装助手 (Qt GUI 应用)              │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌─────────────────────────────────────────────────┐   │
│  │              UI 层 (5 个屏幕)                    │   │
│  ├─────────────────────────────────────────────────┤   │
│  │ • WelcomeScreen      - 欢迎屏幕                 │   │
│  │ • PackageInfoScreen  - 软件包信息               │   │
│  │ • DependencyScreen   - 依赖检查                 │   │
│  │ • InstallScreen      - 安装进度                 │   │
│  │ • CompleteScreen     - 完成屏幕                 │   │
│  └─────────────────────────────────────────────────┘   │
│                         ↓                               │
│  ┌─────────────────────────────────────────────────┐   │
│  │           业务逻辑层 (核心模块)                 │   │
│  ├─────────────────────────────────────────────────┤   │
│  │ • PackageParser      - 软件包解析               │   │
│  │ • DependencyAnalyzer - 依赖分析                 │   │
│  │ • PackageManager     - 包管理器接口             │   │
│  │ • Logger             - 日志系统                 │   │
│  └─────────────────────────────────────────────────┘   │
│                         ↓                               │
│  ┌─────────────────────────────────────────────────┐   │
│  │           系统集成层 (外部调用)                 │   │
│  ├─────────────────────────────────────────────────┤   │
│  │ • tar/unzip          - 压缩包提取               │   │
│  │ • apt/yum/dnf        - 包管理器                 │   │
│  │ • dpkg/rpm           - 包查询                   │   │
│  │ • sudo               - 权限提升                 │   │
│  └─────────────────────────────────────────────────┘   │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

## 数据流设计

### 1. 软件包加载流程

```
用户选择软件包
    ↓
PackageParser::parsePackage()
    ├─ extractArchive()          # 解压压缩包
    ├─ parseMetadata()           # 解析 metadata.json
    └─ parseDependencies()       # 解析 dependencies.json
    ↓
PackageInfoScreen 显示包信息
```

### 2. 依赖分析流程

```
用户确认安装
    ↓
DependencyAnalyzer::buildDependencyTree()
    ├─ 获取所有依赖
    ├─ 检测循环依赖
    ├─ 检查已安装的包
    └─ 生成依赖树
    ↓
DependencyScreen 显示依赖关系
```

### 3. 安装执行流程

```
用户开始安装
    ↓
InstallScreen::performInstallation()
    ├─ PackageManager::detectPackageManager()  # 检测包管理器
    ├─ DependencyAnalyzer::getInstallationOrder()  # 获取安装顺序
    ├─ 对每个包执行:
    │  ├─ PackageManager::installPackage()
    │  ├─ 更新进度条
    │  └─ 记录日志
    └─ 显示安装结果
    ↓
CompleteScreen 显示完成状态
```

## 类设计

### MainWindow (主窗口)

**职责：**
- 管理应用窗口和屏幕切换
- 协调各个屏幕之间的通信
- 保存/恢复窗口状态

**关键方法：**
- `setupConnections()` - 设置信号/槽连接
- `onPackageSelected()` - 处理软件包选择
- `onInstallConfirmed()` - 处理安装确认
- `onInstallCompleted()` - 处理安装完成

### PackageParser (软件包解析器)

**职责：**
- 解析软件包文件
- 提取压缩包
- 读取元数据和依赖信息

**关键方法：**
- `parsePackage()` - 解析整个软件包
- `extractArchive()` - 提取压缩包
- `parseMetadata()` - 解析元数据
- `parseDependencies()` - 解析依赖关系

### DependencyAnalyzer (依赖分析器)

**职责：**
- 分析依赖关系
- 生成安装顺序
- 检测问题

**关键方法：**
- `buildDependencyTree()` - 构建依赖树
- `getInstallationOrder()` - 获取安装顺序
- `hasCyclicDependency()` - 检测循环依赖
- `isPackageInstalled()` - 检查包是否已安装

### PackageManager (包管理器)

**职责：**
- 检测系统包管理器
- 执行安装/卸载操作
- 查询包状态

**关键方法：**
- `detectPackageManager()` - 检测包管理器
- `installPackages()` - 安装软件包
- `isPackageInstalled()` - 检查包状态
- `removePackage()` - 卸载软件包

### Logger (日志系统)

**职责：**
- 记录应用日志
- 提供日志查询接口

**关键方法：**
- `debug()` / `info()` / `warning()` / `error()` - 记录日志
- `getAllLogs()` - 获取所有日志
- `getLogFilePath()` - 获取日志文件路径

## 信号/槽设计

### 屏幕间通信

```
WelcomeScreen
    ↓ packageSelected
PackageInfoScreen
    ├─ installConfirmed → DependencyScreen
    └─ backClicked → WelcomeScreen

DependencyScreen
    ├─ installConfirmed → InstallScreen
    └─ backClicked → PackageInfoScreen

InstallScreen
    ↓ installCompleted
CompleteScreen
    ↓ restartClicked
WelcomeScreen
```

## 错误处理策略

### 1. 解析错误

```cpp
if (!parser.parsePackage(packagePath)) {
    logger->error(parser.getErrorMessage());
    // 显示错误对话框
    // 返回欢迎屏幕
}
```

### 2. 依赖错误

```cpp
if (analyzer.hasCyclicDependency(dependencies)) {
    logger->error("检测到循环依赖");
    // 显示错误信息
    // 不允许继续安装
}
```

### 3. 安装错误

```cpp
if (!packageManager.installPackage(packagePath)) {
    logger->error(packageManager.getErrorMessage());
    // 记录失败的包
    // 尝试回滚已安装的包
    // 显示错误报告
}
```

## 性能优化

### 1. 异步操作

- 使用 QThread 在后台执行耗时操作
- 避免阻塞 UI 线程
- 通过信号/槽进行线程间通信

### 2. 缓存机制

- 缓存解析结果
- 缓存依赖分析结果
- 缓存最近使用的软件包列表

### 3. 内存管理

- 使用智能指针管理资源
- 及时释放临时文件
- 避免内存泄漏

## 安全考虑

### 1. 权限管理

- 需要 root 权限才能安装软件
- 使用 sudo 或 PolicyKit 提升权限
- 验证用户身份

### 2. 文件验证

- 验证软件包完整性 (SHA256 校验和)
- 检查文件权限
- 防止目录遍历攻击

### 3. 输入验证

- 验证软件包路径
- 验证 JSON 文件格式
- 检查包名称合法性

## 扩展性设计

### 1. 新增包管理器

在 `PackageManager` 中添加新的包管理器类型，无需修改其他代码。

### 2. 新增 UI 屏幕

继承 `QWidget` 创建新屏幕，通过信号/槽与其他屏幕通信。

### 3. 新增功能模块

创建新的模块类，通过依赖注入与其他模块集成。

## 测试策略

### 单元测试

- 测试 PackageParser 的解析功能
- 测试 DependencyAnalyzer 的分析功能
- 测试 PackageManager 的命令执行

### 集成测试

- 测试完整的安装流程
- 测试错误处理
- 测试不同包管理器

### UI 测试

- 测试屏幕切换
- 测试用户交互
- 测试错误提示

