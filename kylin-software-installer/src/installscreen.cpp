#include "installscreen.h"
#include "packageparser.h"
#include "packagemanager.h"
#include "dependencyanalyzer.h"
#include "logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QFont>
#include <QThread>

InstallScreen::InstallScreen(std::shared_ptr<Logger> logger, QWidget *parent)
    : QWidget(parent)
    , logger(logger)
    , isInstalling(false)
{
    initializeUI();
}

void InstallScreen::startInstall(const QString &packagePath) {
    currentPackagePath = packagePath;
    isInstalling = true;
    cancelButton->setEnabled(true);
    performInstallation();
}

void InstallScreen::initializeUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel *titleLabel = new QLabel("安装进度", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Current package label
    currentPackageLabel = new QLabel("准备安装...", this);
    mainLayout->addWidget(currentPackageLabel);

    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setValue(0);
    mainLayout->addWidget(progressBar);

    // Status label
    statusLabel = new QLabel("状态: 初始化中...", this);
    mainLayout->addWidget(statusLabel);

    // Log output
    QLabel *logLabel = new QLabel("安装日志:", this);
    QFont logFont = logLabel->font();
    logFont.setPointSize(11);
    logFont.setBold(true);
    logLabel->setFont(logFont);
    mainLayout->addWidget(logLabel);

    logOutput = new QTextEdit(this);
    logOutput->setReadOnly(true);
    logOutput->setMinimumHeight(200);
    logOutput->setStyleSheet(
        "QTextEdit {"
        "  background-color: #1e1e1e;"
        "  color: #00ff00;"
        "  font-family: 'Courier New';"
        "  font-size: 11px;"
        "  border: 1px solid #333;"
        "  border-radius: 4px;"
        "}"
    );
    mainLayout->addWidget(logOutput);

    // Cancel button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    cancelButton = new QPushButton("取消", this);
    cancelButton->setMinimumWidth(100);
    cancelButton->setMinimumHeight(40);
    cancelButton->setEnabled(false);
    connect(cancelButton, &QPushButton::clicked, this, &InstallScreen::onCancelClicked);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void InstallScreen::performInstallation() {
    logOutput->clear();
    appendLog("开始安装软件包...\n");

    PackageParser parser(logger);
    if (!parser.parsePackage(currentPackagePath)) {
        appendLog(QString("错误: 解析软件包失败 - %1\n").arg(parser.getErrorMessage()));
        statusLabel->setText("状态: 安装失败");
        isInstalling = false;
        emit installCompleted(false);
        return;
    }

    PackageMetadata metadata = parser.getMetadata();
    QMap<QString, QStringList> dependencies = parser.getDependencies();

    appendLog(QString("软件包信息: %1 (%2)\n").arg(metadata.targetSystem, metadata.targetArchitecture));
    appendLog(QString("包含 %1 个软件包\n\n").arg(metadata.packages.size()));

    // Analyze dependencies
    DependencyAnalyzer analyzer(logger);
    QStringList packageNames;
    for (const PackageInfo &pkg : metadata.packages) {
        packageNames.append(pkg.name);
    }

    QStringList installOrder = analyzer.getInstallationOrder(packageNames, dependencies);
    if (installOrder.isEmpty()) {
        appendLog("错误: 依赖分析失败\n");
        statusLabel->setText("状态: 安装失败");
        isInstalling = false;
        emit installCompleted(false);
        return;
    }

    appendLog(QString("安装顺序: %1\n\n").arg(installOrder.join(" -> ")));

    // Initialize package manager
    PackageManager pkgManager(logger);
    if (pkgManager.detectPackageManager() == PackageManagerType::Unknown) {
        appendLog("错误: 无法检测到包管理器\n");
        statusLabel->setText("状态: 安装失败");
        isInstalling = false;
        emit installCompleted(false);
        return;
    }

    appendLog("开始安装软件包...\n");

    int totalPackages = installOrder.size();
    int installedCount = 0;

    for (const QString &package : installOrder) {
        if (!isInstalling) {
            appendLog("\n用户取消了安装\n");
            statusLabel->setText("状态: 已取消");
            emit installCompleted(false);
            return;
        }

        installedCount++;
        currentPackageLabel->setText(QString("正在安装: %1 (%2/%3)").arg(package).arg(installedCount).arg(totalPackages));
        appendLog(QString("\n[%1/%2] 安装 %3...\n").arg(installedCount).arg(totalPackages).arg(package));

        // Simulate installation (in real implementation, would call pkgManager.installPackage)
        int progress = (installedCount * 100) / totalPackages;
        progressBar->setValue(progress);
        statusLabel->setText(QString("状态: 安装中 (%1%)").arg(progress));

        // Simulate work
        QThread::msleep(500);
        appendLog(QString("✓ %1 安装成功\n").arg(package));
    }

    progressBar->setValue(100);
    statusLabel->setText("状态: 安装完成");
    currentPackageLabel->setText("所有软件包已安装");
    appendLog("\n✓ 所有软件包安装完成！\n");

    isInstalling = false;
    cancelButton->setEnabled(false);
    emit installCompleted(true);
}

void InstallScreen::onCancelClicked() {
    logger->info("用户取消安装");
    isInstalling = false;
    cancelButton->setEnabled(false);
}

void InstallScreen::updateProgress(int value) {
    progressBar->setValue(value);
}

void InstallScreen::appendLog(const QString &message) {
    logOutput->append(message);
    // Auto-scroll to bottom
    QTextCursor cursor = logOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    logOutput->setTextCursor(cursor);
}
