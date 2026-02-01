#ifndef DEPENDENCYSCREEN_H
#define DEPENDENCYSCREEN_H

#include <QWidget>
#include <memory>

class QTreeWidget;
class QLabel;
class QPushButton;
class Logger;

class DependencyScreen : public QWidget {
    Q_OBJECT

public:
    explicit DependencyScreen(std::shared_ptr<Logger> logger, QWidget *parent = nullptr);

    void analyzeDependencies(const QString &packagePath);

signals:
    void installConfirmed();
    void backClicked();

private slots:
    void onInstallClicked();
    void onBackClicked();

private:
    void initializeUI();
    void displayDependencyTree();

    std::shared_ptr<Logger> logger;
    QString currentPackagePath;

    QLabel *statusLabel;
    QTreeWidget *dependencyTree;
    QPushButton *installButton;
    QPushButton *backButton;
};

#endif // DEPENDENCYSCREEN_H
