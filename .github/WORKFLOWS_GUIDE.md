# GitHub Actions 工作流配置指南

本项目使用 GitHub Actions 自动构建 APK 和 DEB 包。

## 工作流说明

### 1. build-apk-eas.yml - 使用 EAS 构建 APK

**触发条件：**
- 推送到 main/master 分支
- 修改 kylin-software-downloader 目录下的文件
- 手动触发 (workflow_dispatch)

**所需配置：**
- 需要在 GitHub Secrets 中设置 `EXPO_TOKEN`

**获取 EXPO_TOKEN 的步骤：**
1. 访问 https://expo.dev/
2. 创建或登录 Expo 账户
3. 进入 Account Settings → Access Tokens
4. 创建新的 token
5. 在 GitHub 仓库 Settings → Secrets and variables → Actions 中添加 `EXPO_TOKEN`

**构建配置文件：**
- 需要在 kylin-software-downloader 目录下创建 `eas.json` 文件

### 2. build-deb.yml - 构建 DEB 包

**触发条件：**
- 推送到 main/master 分支
- 修改 kylin-software-installer 目录下的文件
- 手动触发 (workflow_dispatch)

**特点：**
- 自动安装所有必需的依赖
- 编译 Qt 应用
- 生成 DEB 包
- 上传到 Artifacts

### 3. build-apk.yml - 备用 APK 构建工作流

**触发条件：**
- 推送 tag（v* 格式）
- 手动触发

## 使用方法

### 自动构建

1. **推送代码到 main/master 分支**
   ```bash
   git push origin main
   ```

2. **查看构建进度**
   - 进入 GitHub 仓库
   - 点击 "Actions" 标签
   - 查看最新的工作流运行

3. **下载构建产物**
   - 点击工作流运行
   - 在 "Artifacts" 部分下载 APK 或 DEB 包

### 手动构建

1. **进入 Actions 标签**
2. **选择要运行的工作流**
3. **点击 "Run workflow" 按钮**
4. **选择参数（如果需要）**
5. **点击 "Run workflow"**

### 发布 Release

1. **创建 tag**
   ```bash
   git tag v1.0.1
   git push origin v1.0.1
   ```

2. **工作流自动运行**
   - 构建 APK 和 DEB
   - 创建 GitHub Release
   - 上传构建产物

## 配置 eas.json

在 `kylin-software-downloader` 目录下创建 `eas.json`：

```json
{
  "cli": {
    "version": ">= 5.0.0"
  },
  "build": {
    "preview": {
      "android": {
        "buildType": "apk"
      }
    },
    "production": {
      "android": {
        "buildType": "app-bundle"
      }
    }
  },
  "submit": {
    "production": {
      "android": {
        "serviceAccount": "path/to/service-account.json"
      }
    }
  }
}
```

## 故障排除

### 构建失败

1. **检查日志**
   - 进入 Actions 页面
   - 点击失败的工作流
   - 查看详细的错误信息

2. **常见问题**
   - EXPO_TOKEN 未设置或过期
   - 依赖版本不兼容
   - 代码编译错误

### 重新运行工作流

1. 进入 Actions 页面
2. 点击失败的工作流
3. 点击 "Re-run all jobs" 按钮

## 环境变量

### 必需的 Secrets

| 名称 | 描述 | 获取方式 |
|------|------|--------|
| EXPO_TOKEN | Expo 访问令牌 | https://expo.dev/settings/tokens |

### 可选的 Secrets

| 名称 | 描述 | 用途 |
|------|------|------|
| GITHUB_TOKEN | GitHub 自动生成 | 自动上传到 Release |

## 最佳实践

1. **定期更新依赖**
   ```bash
   npm update
   ```

2. **测试本地构建**
   ```bash
   cd kylin-software-downloader
   npm run build
   ```

3. **在 tag 上创建 Release**
   ```bash
   git tag -a v1.0.0 -m "Release v1.0.0"
   git push origin v1.0.0
   ```

4. **监控工作流运行**
   - 定期检查 Actions 页面
   - 订阅工作流通知

## 更多信息

- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [Expo EAS Build 文档](https://docs.expo.dev/build/introduction/)
- [CMake 文档](https://cmake.org/documentation/)
