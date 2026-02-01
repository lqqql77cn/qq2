#ifndef INSTALLSCREEN_H
#define INSTALLSCREEN_H

#include <QWidget>
#include <memory>

class QProgressBar;
class QLabel;
class QTextEdit;
class QPushButton;
class Logger;

class InstallScreen : public QWidget {
    Q_OBJECT

public:
    explicit InstallScreen(std::shared_ptr<Logger> logger, QWidget *parent = nullptr);

    void startInstall(const QString &packagePath);

signals:
    void installCompleted(bool success);

private slots:
    void onCancelClicked();
    void updateProgress(int value);
    void appendLog(const QString &message);

private:
    void initializeUI();
    void performInstallation();

    std::shared_ptr<Logger> logger;
    QString currentPackagePath;

    QLabel *currentPackageLabel;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QTextEdit *logOutput;
    QPushButton *cancelButton;

    bool isInstalling;
};

#endif // INSTALLSCREEN_H
