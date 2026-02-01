#include "mainwindow.h"
#include "welcomescreen.h"
#include "packageinfoscreen.h"
#include "dependencyscreen.h"
#include "installscreen.h"
#include "completescreen.h"
#include "logger.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QSettings>
#include <QScreen>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , logger(std::make_shared<Logger>())
{
    setWindowTitle("银河麒麟软件安装助手");
    setWindowIcon(QIcon(":/icons/app.png"));
    
    // Set minimum window size
    setMinimumSize(800, 600);
    
    // Initialize UI
    initializeUI();
    setupConnections();
    restoreWindowState();
    
    logger->info("应用启动成功");
}

MainWindow::~MainWindow() {
    saveWindowState();
}

void MainWindow::initializeUI() {
    // Create central widget and stacked widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    stackedWidget = new QStackedWidget(this);
    
    // Create screens
    welcomeScreen = std::make_unique<WelcomeScreen>(logger, this);
    packageInfoScreen = std::make_unique<PackageInfoScreen>(logger, this);
    dependencyScreen = std::make_unique<DependencyScreen>(logger, this);
    installScreen = std::make_unique<InstallScreen>(logger, this);
    completeScreen = std::make_unique<CompleteScreen>(logger, this);
    
    // Add screens to stacked widget
    stackedWidget->addWidget(welcomeScreen.get());
    stackedWidget->addWidget(packageInfoScreen.get());
    stackedWidget->addWidget(dependencyScreen.get());
    stackedWidget->addWidget(installScreen.get());
    stackedWidget->addWidget(completeScreen.get());
    
    layout->addWidget(stackedWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::setupConnections() {
    // Welcome screen signals
    connect(welcomeScreen.get(), &WelcomeScreen::packageSelected,
            this, &MainWindow::onPackageSelected);
    
    // Package info screen signals
    connect(packageInfoScreen.get(), &PackageInfoScreen::installConfirmed,
            this, &MainWindow::onInstallConfirmed);
    connect(packageInfoScreen.get(), &PackageInfoScreen::backClicked,
            welcomeScreen.get(), [this]() { stackedWidget->setCurrentWidget(welcomeScreen.get()); });
    
    // Dependency screen signals
    connect(dependencyScreen.get(), &DependencyScreen::installConfirmed,
            this, &MainWindow::onInstallConfirmed);
    connect(dependencyScreen.get(), &DependencyScreen::backClicked,
            packageInfoScreen.get(), [this]() { stackedWidget->setCurrentWidget(packageInfoScreen.get()); });
    
    // Install screen signals
    connect(installScreen.get(), &InstallScreen::installCompleted,
            this, &MainWindow::onInstallCompleted);
    
    // Complete screen signals
    connect(completeScreen.get(), &CompleteScreen::restartClicked,
            welcomeScreen.get(), [this]() { stackedWidget->setCurrentWidget(welcomeScreen.get()); });
    
    // Stacked widget signals
    connect(stackedWidget, QOverload<int>::of(&QStackedWidget::currentChanged),
            this, &MainWindow::onScreenChanged);
}

void MainWindow::onPackageSelected(const QString &packagePath) {
    currentPackagePath = packagePath;
    packageInfoScreen->loadPackage(packagePath);
    stackedWidget->setCurrentWidget(packageInfoScreen.get());
    logger->info(QString("选择软件包: %1").arg(packagePath));
}

void MainWindow::onInstallConfirmed() {
    // Move to dependency screen
    dependencyScreen->analyzeDependencies(currentPackagePath);
    stackedWidget->setCurrentWidget(dependencyScreen.get());
}

void MainWindow::onInstallCompleted(bool success) {
    completeScreen->setInstallResult(success, currentPackagePath);
    stackedWidget->setCurrentWidget(completeScreen.get());
}

void MainWindow::onScreenChanged(int index) {
    logger->debug(QString("屏幕切换到索引: %1").arg(index));
}

void MainWindow::restoreWindowState() {
    QSettings settings("Kylin", "SoftwareInstaller");
    
    // Restore window geometry
    QRect geometry = settings.value("geometry", QRect()).toRect();
    if (geometry.isValid()) {
        setGeometry(geometry);
    } else {
        // Center on screen
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
    
    // Restore window state
    if (settings.value("maximized", false).toBool()) {
        showMaximized();
    } else {
        show();
    }
}

void MainWindow::saveWindowState() {
    QSettings settings("Kylin", "SoftwareInstaller");
    settings.setValue("geometry", geometry());
    settings.setValue("maximized", isMaximized());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWindowState();
    event->accept();
}
