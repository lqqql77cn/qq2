#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <memory>

class WelcomeScreen;
class PackageInfoScreen;
class DependencyScreen;
class InstallScreen;
class CompleteScreen;
class Logger;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onPackageSelected(const QString &packagePath);
    void onInstallConfirmed();
    void onInstallCompleted(bool success);
    void onScreenChanged(int index);

private:
    void initializeUI();
    void setupConnections();
    void restoreWindowState();
    void saveWindowState();

    QStackedWidget *stackedWidget;
    std::unique_ptr<WelcomeScreen> welcomeScreen;
    std::unique_ptr<PackageInfoScreen> packageInfoScreen;
    std::unique_ptr<DependencyScreen> dependencyScreen;
    std::unique_ptr<InstallScreen> installScreen;
    std::unique_ptr<CompleteScreen> completeScreen;
    std::shared_ptr<Logger> logger;

    QString currentPackagePath;
};

#endif // MAINWINDOW_H
