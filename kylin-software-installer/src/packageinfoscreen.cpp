#include "packageinfoscreen.h"
#include "packageparser.h"
#include "logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>

PackageInfoScreen::PackageInfoScreen(std::shared_ptr<Logger> logger, QWidget *parent)
    : QWidget(parent)
    , logger(logger)
{
    initializeUI();
}

void PackageInfoScreen::loadPackage(const QString &packagePath) {
    currentPackagePath = packagePath;

    PackageParser parser(logger);
    if (parser.parsePackage(packagePath)) {
        displayPackageInfo();
    } else {
        logger->error(QString("解析软件包失败: %1").arg(parser.getErrorMessage()));
    }
}

void PackageInfoScreen::initializeUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel *titleLabel = new QLabel("软件包信息", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Package info section
    QLabel *infoLabel = new QLabel("软件包详情:", this);
    QFont infoFont = infoLabel->font();
    infoFont.setPointSize(12);
    infoFont.setBold(true);
    infoLabel->setFont(infoFont);
    mainLayout->addWidget(infoLabel);

    // Package details
    packageNameLabel = new QLabel("软件包名称: -", this);
    versionLabel = new QLabel("版本: -", this);
    systemLabel = new QLabel("目标系统: -", this);
    architectureLabel = new QLabel("目标架构: -", this);
    sizeLabel = new QLabel("总大小: -", this);

    mainLayout->addWidget(packageNameLabel);
    mainLayout->addWidget(versionLabel);
    mainLayout->addWidget(systemLabel);
    mainLayout->addWidget(architectureLabel);
    mainLayout->addWidget(sizeLabel);

    // Packages list section
    QLabel *packagesLabel = new QLabel("包含的软件包:", this);
    QFont packagesFont = packagesLabel->font();
    packagesFont.setPointSize(12);
    packagesFont.setBold(true);
    packagesLabel->setFont(packagesFont);
    mainLayout->addWidget(packagesLabel);

    packagesList = new QListWidget(this);
    packagesList->setMinimumHeight(200);
    mainLayout->addWidget(packagesList);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    backButton = new QPushButton("返回", this);
    backButton->setMinimumWidth(100);
    backButton->setMinimumHeight(40);
    connect(backButton, &QPushButton::clicked, this, &PackageInfoScreen::onBackClicked);
    buttonLayout->addWidget(backButton);

    installButton = new QPushButton("继续安装", this);
    installButton->setMinimumWidth(100);
    installButton->setMinimumHeight(40);
    installButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #0066CC;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0052A3;"
        "}"
    );
    connect(installButton, &QPushButton::clicked, this, &PackageInfoScreen::onInstallClicked);
    buttonLayout->addWidget(installButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void PackageInfoScreen::displayPackageInfo() {
    PackageParser parser(logger);
    if (!parser.parsePackage(currentPackagePath)) {
        return;
    }

    PackageMetadata metadata = parser.getMetadata();

    // Update labels
    packageNameLabel->setText(QString("软件包名称: %1").arg(metadata.version));
    versionLabel->setText(QString("版本: %1").arg(metadata.timestamp));
    systemLabel->setText(QString("目标系统: %1").arg(metadata.targetSystem));
    architectureLabel->setText(QString("目标架构: %1").arg(metadata.targetArchitecture));

    // Format size
    QString sizeStr;
    if (metadata.totalSize > 1024 * 1024 * 1024) {
        sizeStr = QString::number(metadata.totalSize / (1024.0 * 1024 * 1024), 'f', 2) + " GB";
    } else if (metadata.totalSize > 1024 * 1024) {
        sizeStr = QString::number(metadata.totalSize / (1024.0 * 1024), 'f', 2) + " MB";
    } else {
        sizeStr = QString::number(metadata.totalSize / 1024.0, 'f', 2) + " KB";
    }
    sizeLabel->setText(QString("总大小: %1").arg(sizeStr));

    // Display packages
    packagesList->clear();
    for (const PackageInfo &pkg : metadata.packages) {
        QString itemText = QString("%1 (%2)").arg(pkg.name, pkg.version);
        packagesList->addItem(itemText);
    }
}

void PackageInfoScreen::onInstallClicked() {
    logger->info("用户确认安装");
    emit installConfirmed();
}

void PackageInfoScreen::onBackClicked() {
    logger->info("用户返回");
    emit backClicked();
}
