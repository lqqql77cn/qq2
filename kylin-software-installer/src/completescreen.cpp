#include "completescreen.h"
#include "logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QFont>
#include <QDateTime>

CompleteScreen::CompleteScreen(std::shared_ptr<Logger> logger, QWidget *parent)
    : QWidget(parent)
    , logger(logger)
{
    initializeUI();
}

void CompleteScreen::setInstallResult(bool success, const QString &packagePath) {
    if (success) {
        statusLabel->setText("✓ 安装成功");
        statusLabel->setStyleSheet("color: #22C55E; font-size: 18px; font-weight: bold;");
        summaryLabel->setText("所有软件包已成功安装到系统中。");
    } else {
        statusLabel->setText("✗ 安装失败");
        statusLabel->setStyleSheet("color: #EF4444; font-size: 18px; font-weight: bold;");
        summaryLabel->setText("安装过程中出现错误，请查看详细日志。");
    }

    // Display installation details
    QString details;
    details += QString("安装时间: %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    details += QString("软件包: %1\n").arg(packagePath);
    details += QString("状态: %1\n\n").arg(success ? "成功" : "失败");

    // Get logs
    QString logs = logger->getAllLogs();
    details += "安装日志:\n";
    details += logs;

    detailsOutput->setPlainText(details);
}

void CompleteScreen::initializeUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel *titleLabel = new QLabel("安装完成", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Status label
    statusLabel = new QLabel("处理中...", this);
    mainLayout->addWidget(statusLabel);

    // Summary label
    summaryLabel = new QLabel("", this);
    summaryLabel->setStyleSheet("color: #666; font-size: 14px;");
    mainLayout->addWidget(summaryLabel);

    // Details section
    QLabel *detailsLabel = new QLabel("安装详情:", this);
    QFont detailsFont = detailsLabel->font();
    detailsFont.setPointSize(11);
    detailsFont.setBold(true);
    detailsLabel->setFont(detailsFont);
    mainLayout->addWidget(detailsLabel);

    detailsOutput = new QTextEdit(this);
    detailsOutput->setReadOnly(true);
    detailsOutput->setMinimumHeight(250);
    detailsOutput->setStyleSheet(
        "QTextEdit {"
        "  background-color: #F9F9F9;"
        "  color: #333;"
        "  font-family: 'Courier New';"
        "  font-size: 10px;"
        "  border: 1px solid #DDD;"
        "  border-radius: 4px;"
        "}"
    );
    mainLayout->addWidget(detailsOutput);

    // Restart button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    restartButton = new QPushButton("返回首页", this);
    restartButton->setMinimumWidth(120);
    restartButton->setMinimumHeight(40);
    restartButton->setStyleSheet(
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
    connect(restartButton, &QPushButton::clicked, this, &CompleteScreen::onRestartClicked);
    buttonLayout->addWidget(restartButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void CompleteScreen::onRestartClicked() {
    logger->info("用户返回首页");
    emit restartClicked();
}
