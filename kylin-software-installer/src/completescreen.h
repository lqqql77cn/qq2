#ifndef COMPLETESCREEN_H
#define COMPLETESCREEN_H

#include <QWidget>
#include <memory>

class QLabel;
class QPushButton;
class QTextEdit;
class Logger;

class CompleteScreen : public QWidget {
    Q_OBJECT

public:
    explicit CompleteScreen(std::shared_ptr<Logger> logger, QWidget *parent = nullptr);

    void setInstallResult(bool success, const QString &packagePath);

signals:
    void restartClicked();

private slots:
    void onRestartClicked();

private:
    void initializeUI();

    std::shared_ptr<Logger> logger;

    QLabel *statusLabel;
    QLabel *summaryLabel;
    QTextEdit *detailsOutput;
    QPushButton *restartButton;
};

#endif // COMPLETESCREEN_H
