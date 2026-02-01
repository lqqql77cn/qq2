#ifndef PACKAGEINFOSCREEN_H
#define PACKAGEINFOSCREEN_H

#include <QWidget>
#include <memory>

class QLabel;
class QPushButton;
class QListWidget;
class Logger;

class PackageInfoScreen : public QWidget {
    Q_OBJECT

public:
    explicit PackageInfoScreen(std::shared_ptr<Logger> logger, QWidget *parent = nullptr);

    void loadPackage(const QString &packagePath);

signals:
    void installConfirmed();
    void backClicked();

private slots:
    void onInstallClicked();
    void onBackClicked();

private:
    void initializeUI();
    void displayPackageInfo();

    std::shared_ptr<Logger> logger;
    QString currentPackagePath;

    QLabel *packageNameLabel;
    QLabel *versionLabel;
    QLabel *systemLabel;
    QLabel *architectureLabel;
    QLabel *sizeLabel;
    QListWidget *packagesList;
    QPushButton *installButton;
    QPushButton *backButton;
};

#endif // PACKAGEINFOSCREEN_H
