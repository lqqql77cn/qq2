#include "welcomescreen.h"
#include "logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QSettings>
#include <QFont>

WelcomeScreen::WelcomeScreen(std::shared_ptr<Logger> logger, QWidget *parent)
    : QWidget(parent)
    , logger(logger)
{
    initializeUI();
    loadRecentPackages();
}

void WelcomeScreen::initializeUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel *titleLabel = new QLabel("银河麒麟软件安装助手", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Description
    QLabel *descLabel = new QLabel(
        "欢迎使用银河麒麟软件安装助手。\n"
        "此工具用于安装从安卓下载端打包的软件包。\n"
        "请选择要安装的软件包文件。",
        this
    );
    descLabel->setStyleSheet("color: #666; font-size: 14px;");
    mainLayout->addWidget(descLabel);

    // Select package button
    selectPackageButton = new QPushButton("选择软件包文件", this);
    selectPackageButton->setMinimumHeight(50);
    selectPackageButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #0066CC;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0052A3;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #003D7A;"
        "}"
    );
    connect(selectPackageButton, &QPushButton::clicked, this, &WelcomeScreen::onSelectPackageClicked);
    mainLayout->addWidget(selectPackageButton);

    // Recent packages section
    QLabel *recentLabel = new QLabel("最近使用的软件包:", this);
    QFont recentFont = recentLabel->font();
    recentFont.setPointSize(12);
    recentFont.setBold(true);
    recentLabel->setFont(recentFont);
    mainLayout->addWidget(recentLabel);

    // Recent packages list
    recentPackagesList = new QListWidget(this);
    recentPackagesList->setMinimumHeight(150);
    recentPackagesList->setStyleSheet(
        "QListWidget {"
        "  border: 1px solid #DDD;"
        "  border-radius: 8px;"
        "  background-color: #F9F9F9;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: #E8F0FE;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #0066CC;"
        "  color: white;"
        "}"
    );
    connect(recentPackagesList, &QListWidget::itemClicked, this, &WelcomeScreen::onRecentPackageClicked);
    mainLayout->addWidget(recentPackagesList);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void WelcomeScreen::loadRecentPackages() {
    recentPackagesList->clear();

    QSettings settings("Kylin", "SoftwareInstaller");
    QStringList recentPackages = settings.value("recentPackages", QStringList()).toStringList();

    for (const QString &package : recentPackages) {
        recentPackagesList->addItem(package);
    }

    if (recentPackages.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("暂无最近使用的软件包", recentPackagesList);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }
}

void WelcomeScreen::onSelectPackageClicked() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择软件包文件",
        QString(),
        "压缩包文件 (*.tar.gz *.zip);;所有文件 (*)"
    );

    if (!fileName.isEmpty()) {
        // Add to recent packages
        QSettings settings("Kylin", "SoftwareInstaller");
        QStringList recentPackages = settings.value("recentPackages", QStringList()).toStringList();
        recentPackages.removeAll(fileName);
        recentPackages.prepend(fileName);
        if (recentPackages.size() > 10) {
            recentPackages.removeLast();
        }
        settings.setValue("recentPackages", recentPackages);

        logger->info(QString("用户选择软件包: %1").arg(fileName));
        emit packageSelected(fileName);
    }
}

void WelcomeScreen::onRecentPackageClicked() {
    QListWidgetItem *item = recentPackagesList->currentItem();
    if (item) {
        QString packagePath = item->text();
        logger->info(QString("用户选择最近的软件包: %1").arg(packagePath));
        emit packageSelected(packagePath);
    }
}
