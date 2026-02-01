#include "dependencyscreen.h"
#include "packageparser.h"
#include "dependencyanalyzer.h"
#include "logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFont>

DependencyScreen::DependencyScreen(std::shared_ptr<Logger> logger, QWidget *parent)
    : QWidget(parent)
    , logger(logger)
{
    initializeUI();
}

void DependencyScreen::analyzeDependencies(const QString &packagePath) {
    currentPackagePath = packagePath;
    displayDependencyTree();
}

void DependencyScreen::initializeUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel *titleLabel = new QLabel("依赖关系检查", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Status label
    statusLabel = new QLabel("分析依赖关系中...", this);
    mainLayout->addWidget(statusLabel);

    // Dependency tree
    dependencyTree = new QTreeWidget(this);
    dependencyTree->setHeaderLabels(QStringList() << "软件包" << "状态" << "版本");
    dependencyTree->setMinimumHeight(300);
    mainLayout->addWidget(dependencyTree);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    backButton = new QPushButton("返回", this);
    backButton->setMinimumWidth(100);
    backButton->setMinimumHeight(40);
    connect(backButton, &QPushButton::clicked, this, &DependencyScreen::onBackClicked);
    buttonLayout->addWidget(backButton);

    installButton = new QPushButton("开始安装", this);
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
    connect(installButton, &QPushButton::clicked, this, &DependencyScreen::onInstallClicked);
    buttonLayout->addWidget(installButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void DependencyScreen::displayDependencyTree() {
    dependencyTree->clear();

    PackageParser parser(logger);
    if (!parser.parsePackage(currentPackagePath)) {
        statusLabel->setText(QString("错误: %1").arg(parser.getErrorMessage()));
        return;
    }

    PackageMetadata metadata = parser.getMetadata();
    QMap<QString, QStringList> dependencies = parser.getDependencies();

    DependencyAnalyzer analyzer(logger);

    // Get package names
    QStringList packageNames;
    for (const PackageInfo &pkg : metadata.packages) {
        packageNames.append(pkg.name);
    }

    // Build dependency tree
    auto depTree = analyzer.buildDependencyTree(packageNames, dependencies);

    // Display tree
    int installedCount = 0;
    int totalCount = 0;

    for (const auto &node : depTree) {
        totalCount++;
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, node.name);
        item->setText(1, node.installed ? "已安装" : "待安装");
        item->setText(2, "");

        if (node.installed) {
            installedCount++;
            item->setForeground(0, Qt::gray);
        }

        dependencyTree->addTopLevelItem(item);
    }

    statusLabel->setText(QString("依赖分析完成: 共 %1 个包，其中 %2 个已安装")
                         .arg(totalCount).arg(installedCount));

    logger->info(QString("依赖分析完成: 总计 %1 个包，已安装 %2 个").arg(totalCount).arg(installedCount));
}

void DependencyScreen::onInstallClicked() {
    logger->info("用户确认开始安装");
    emit installConfirmed();
}

void DependencyScreen::onBackClicked() {
    logger->info("用户返回");
    emit backClicked();
}
