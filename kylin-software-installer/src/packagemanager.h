#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <QString>
#include <QStringList>
#include <memory>

class Logger;

enum class PackageManagerType {
    APT,    // Debian/Ubuntu
    YUM,    // RedHat/CentOS
    DNF,    // Fedora
    Unknown
};

class PackageManager {
public:
    explicit PackageManager(std::shared_ptr<Logger> logger);

    // Detect system package manager
    PackageManagerType detectPackageManager();
    
    // Install package
    bool installPackage(const QString &packagePath);
    
    // Install multiple packages
    bool installPackages(const QStringList &packagePaths);
    
    // Check if package is installed
    bool isPackageInstalled(const QString &packageName);
    
    // Remove package
    bool removePackage(const QString &packageName);
    
    // Update package database
    bool updatePackageDatabase();
    
    // Get error message
    QString getErrorMessage() const;
    
    // Get package manager type string
    static QString packageManagerTypeToString(PackageManagerType type);

private:
    bool executeCommand(const QString &command, const QStringList &arguments);
    QString getPackageNameFromPath(const QString &packagePath);

    std::shared_ptr<Logger> logger;
    PackageManagerType currentPackageManager;
    QString errorMessage;
};

#endif // PACKAGEMANAGER_H
