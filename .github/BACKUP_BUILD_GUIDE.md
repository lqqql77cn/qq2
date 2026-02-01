# 备用构建工作流 (build-apk.yml) 使用指南

## 概述

`build-apk.yml` 是一个备用的 APK 构建工作流，与主要的 `build-apk-eas.yml` 相比有以下特点：

| 特性 | build-apk-eas.yml | build-apk.yml |
|------|------------------|---------------|
| 触发方式 | 推送代码到 main/master | 推送 tag 或手动触发 |
| 构建时机 | 每次推送都构建 | 仅在发布版本时构建 |
| 用途 | 开发/测试构建 | 生产/发布构建 |
| Release 上传 | 否 | 是 |
| 自动化程度 | 高 | 高 |

## 使用场景

### 场景 1：发布新版本到 Release

**步骤：**

1. **确保代码已提交**
   ```bash
   git add .
   git commit -m "Release v1.0.1: Add new features"
   ```

2. **创建版本 tag**
   ```bash
   git tag -a v1.0.1 -m "Release version 1.0.1"
   ```

3. **推送 tag 到 GitHub**
   ```bash
   git push origin v1.0.1
   ```

4. **工作流自动运行**
   - GitHub 检测到 `v1.0.1` tag
   - 触发 `build-apk.yml` 工作流
   - 自动构建 APK
   - 自动上传到 Release

5. **查看 Release**
   - 进入仓库的 Releases 页面
   - 找到 v1.0.1 Release
   - 下载 APK 文件

### 场景 2：手动触发构建

**步骤：**

1. **进入 GitHub 仓库**
2. **点击 "Actions" 标签**
3. **在左侧选择 "Build APK" 工作流**
4. **点击 "Run workflow" 按钮**
5. **选择分支（通常是 main）**
6. **点击 "Run workflow"**
7. **等待构建完成**
8. **在 Artifacts 中下载 APK**

### 场景 3：修复发布版本中的 bug

**步骤：**

1. **创建 hotfix 分支**
   ```bash
   git checkout -b hotfix/v1.0.2 main
   ```

2. **修复 bug**
   ```bash
   # 编辑文件并修复问题
   git add .
   git commit -m "Fix: Critical bug in download manager"
   ```

3. **合并到 main**
   ```bash
   git checkout main
   git merge hotfix/v1.0.2
   ```

4. **创建新的 tag**
   ```bash
   git tag -a v1.0.2 -m "Hotfix: Critical bug fix"
   git push origin v1.0.2
   ```

5. **工作流自动构建并发布**

## Tag 命名规范

使用语义化版本号命名 tag：

```
v<MAJOR>.<MINOR>.<PATCH>
```

**示例：**
- `v1.0.0` - 首个发布版本
- `v1.1.0` - 新增功能版本
- `v1.0.1` - 修复 bug 版本
- `v2.0.0` - 重大更新版本

**规则：**
- MAJOR：不兼容的 API 更改
- MINOR：向后兼容的新功能
- PATCH：向后兼容的 bug 修复

## 工作流执行流程

```
推送 tag (v*)
    ↓
GitHub 检测到 tag
    ↓
触发 build-apk.yml 工作流
    ↓
检出代码 (Checkout code)
    ↓
设置 Node.js 环境
    ↓
设置 Java 环境
    ↓
安装依赖 (npm ci)
    ↓
使用 EAS 构建 APK
    ↓
从 EAS 获取 APK
    ↓
上传到 GitHub Release
    ↓
完成！
```

## 常见操作

### 查看工作流运行历史

1. 进入仓库的 Actions 标签
2. 在左侧选择 "Build APK"
3. 查看所有运行记录

### 重新运行失败的构建

1. 进入 Actions 页面
2. 找到失败的工作流运行
3. 点击 "Re-run all jobs"
4. 等待重新构建

### 删除错误的 tag

如果推送了错误的 tag，可以删除：

```bash
# 删除本地 tag
git tag -d v1.0.1

# 删除远程 tag
git push origin --delete v1.0.1
```

### 查看 Release

1. 进入仓库主页
2. 点击右侧的 "Releases"
3. 查看所有发布版本
4. 下载对应版本的 APK

## 与 build-apk-eas.yml 的区别

### build-apk-eas.yml（主要工作流）
- ✅ 每次推送代码都构建
- ✅ 用于开发和测试
- ✅ 快速反馈
- ❌ 不自动上传到 Release

### build-apk.yml（备用工作流）
- ✅ 仅在推送 tag 时构建
- ✅ 用于发布版本
- ✅ 自动上传到 Release
- ✅ 保持版本历史

## 故障排除

### 构建失败

**问题：** 工作流显示红色 ❌

**解决方案：**
1. 点击失败的工作流
2. 查看详细的错误日志
3. 常见原因：
   - EXPO_TOKEN 未设置
   - 代码编译错误
   - 依赖版本不兼容

### APK 未上传到 Release

**问题：** 构建成功但 Release 中没有 APK

**解决方案：**
1. 检查 tag 是否以 `v` 开头
2. 检查 Release 是否已创建
3. 查看工作流日志中的 "Upload to Release" 步骤

### 无法推送 tag

**问题：** `git push origin v1.0.1` 失败

**解决方案：**
```bash
# 确保本地 tag 已创建
git tag -l

# 确保有推送权限
git push origin --tags

# 或逐个推送
git push origin v1.0.1
```

## 最佳实践

1. **在发布前测试**
   ```bash
   # 使用 build-apk-eas.yml 进行测试构建
   git push origin main
   ```

2. **编写清晰的 tag 消息**
   ```bash
   git tag -a v1.0.1 -m "Release v1.0.1: Add download history feature"
   ```

3. **更新 CHANGELOG**
   - 在推送 tag 前更新 CHANGELOG.md
   - 记录版本变更内容

4. **验证 Release 信息**
   - 检查 Release 页面
   - 确认 APK 文件已上传
   - 编写 Release 说明

5. **保持版本一致**
   - app.config.ts 中的版本号
   - package.json 中的版本号
   - tag 版本号保持一致

## 示例工作流

### 完整的发布流程

```bash
# 1. 确保在 main 分支
git checkout main

# 2. 更新版本号
# 编辑 app.config.ts 和 package.json

# 3. 更新 CHANGELOG
# 编辑 CHANGELOG.md

# 4. 提交更改
git add .
git commit -m "Bump version to 1.0.1"

# 5. 创建 tag
git tag -a v1.0.1 -m "Release v1.0.1: Add download history"

# 6. 推送代码和 tag
git push origin main
git push origin v1.0.1

# 7. 等待 GitHub Actions 完成
# 进入 Actions 页面查看进度

# 8. 查看 Release
# 进入 Releases 页面下载 APK
```

## 更多信息

- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [Git Tag 文档](https://git-scm.com/book/en/v2/Git-Basics-Tagging)
- [语义化版本](https://semver.org/)
