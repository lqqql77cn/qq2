#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QWidget>
#include <memory>

class QListWidget>
class QPushButton;
class Logger;

class WelcomeScreen : public QWidget {
    Q_OBJECT

public:
    explicit WelcomeScreen(std::shared_ptr<Logger> logger, QWidget *parent = nullptr);

signals:
    void packageSelected(const QString &packagePath);

private slots:
    void onSelectPackageClicked();
    void onRecentPackageClicked();

private:
    void initializeUI();
    void loadRecentPackages();

    std::shared_ptr<Logger> logger;
    QListWidget *recentPackagesList;
    QPushButton *selectPackageButton;
};

#endif // WELCOMESCREEN_H
