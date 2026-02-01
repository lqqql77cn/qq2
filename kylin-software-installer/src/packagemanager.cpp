#include "packagemanager.h"
#include "logger.h"

#include <QProcess>
#include <QFile>
#include <QFileInfo>

PackageManager::PackageManager(std::shared_ptr<Logger> logger)
    : logger(logger)
    , currentPackageManager(PackageManagerType::Unknown)
{
    currentPackageManager = detectPackageManager();
}

PackageManagerType PackageManager::detectPackageManager() {
    logger->info("检测系统包管理器");
    
    // Check for apt (Debian/Ubuntu)
    QProcess process;
    process.start("which", QStringList() << "apt");
    if (process.waitForFinished() && process.exitCode() == 0) {
        logger->info("检测到 APT 包管理器");
        return PackageManagerType::APT;
    }
    
    // Check for yum (RedHat/CentOS)
    process.start("which", QStringList() << "yum");
    if (process.waitForFinished() && process.exitCode() == 0) {
        logger->info("检测到 YUM 包管理器");
        return PackageManagerType::YUM;
    }
    
    // Check for dnf (Fedora)
    process.start("which", QStringList() << "dnf");
    if (process.waitForFinished() && process.exitCode() == 0) {
        logger->info("检测到 DNF 包管理器");
        return PackageManagerType::DNF;
    }
    
    errorMessage = "无法检测到支持的包管理器";
    logger->error(errorMessage);
    return PackageManagerType::Unknown;
}

bool PackageManager::installPackage(const QString &packagePath) {
    return installPackages(QStringList() << packagePath);
}

bool PackageManager::installPackages(const QStringList &packagePaths) {
    if (currentPackageManager == PackageManagerType::Unknown) {
        errorMessage = "未检测到包管理器";
        logger->error(errorMessage);
        return false;
    }
    
    logger->info(QString("开始安装 %1 个软件包").arg(packagePaths.size()));
    
    QString command;
    QStringList arguments;
    
    switch (currentPackageManager) {
    case PackageManagerType::APT:
        command = "sudo";
        arguments << "apt" << "install" << "-y" << packagePaths;
        break;
    case PackageManagerType::YUM:
        command = "sudo";
        arguments << "yum" << "install" << "-y" << packagePaths;
        break;
    case PackageManagerType::DNF:
        command = "sudo";
        arguments << "dnf" << "install" << "-y" << packagePaths;
        break;
    default:
        errorMessage = "未知的包管理器";
        logger->error(errorMessage);
        return false;
    }
    
    return executeCommand(command, arguments);
}

bool PackageManager::isPackageInstalled(const QString &packageName) {
    QProcess process;
    
    switch (currentPackageManager) {
    case PackageManagerType::APT:
        process.start("dpkg", QStringList() << "-l" << packageName);
        break;
    case PackageManagerType::YUM:
    case PackageManagerType::DNF:
        process.start("rpm", QStringList() << "-q" << packageName);
        break;
    default:
        return false;
    }
    
    if (process.waitForFinished()) {
        return process.exitCode() == 0;
    }
    
    return false;
}

bool PackageManager::removePackage(const QString &packageName) {
    if (currentPackageManager == PackageManagerType::Unknown) {
        errorMessage = "未检测到包管理器";
        logger->error(errorMessage);
        return false;
    }
    
    logger->info(QString("开始卸载软件包: %1").arg(packageName));
    
    QString command;
    QStringList arguments;
    
    switch (currentPackageManager) {
    case PackageManagerType::APT:
        command = "sudo";
        arguments << "apt" << "remove" << "-y" << packageName;
        break;
    case PackageManagerType::YUM:
        command = "sudo";
        arguments << "yum" << "remove" << "-y" << packageName;
        break;
    case PackageManagerType::DNF:
        command = "sudo";
        arguments << "dnf" << "remove" << "-y" << packageName;
        break;
    default:
        errorMessage = "未知的包管理器";
        logger->error(errorMessage);
        return false;
    }
    
    return executeCommand(command, arguments);
}

bool PackageManager::updatePackageDatabase() {
    if (currentPackageManager == PackageManagerType::Unknown) {
        errorMessage = "未检测到包管理器";
        logger->error(errorMessage);
        return false;
    }
    
    logger->info("更新包管理器数据库");
    
    QString command;
    QStringList arguments;
    
    switch (currentPackageManager) {
    case PackageManagerType::APT:
        command = "sudo";
        arguments << "apt" << "update";
        break;
    case PackageManagerType::YUM:
        command = "sudo";
        arguments << "yum" << "makecache";
        break;
    case PackageManagerType::DNF:
        command = "sudo";
        arguments << "dnf" << "makecache";
        break;
    default:
        errorMessage = "未知的包管理器";
        logger->error(errorMessage);
        return false;
    }
    
    return executeCommand(command, arguments);
}

QString PackageManager::getErrorMessage() const {
    return errorMessage;
}

QString PackageManager::packageManagerTypeToString(PackageManagerType type) {
    switch (type) {
    case PackageManagerType::APT:
        return "APT";
    case PackageManagerType::YUM:
        return "YUM";
    case PackageManagerType::DNF:
        return "DNF";
    default:
        return "Unknown";
    }
}

bool PackageManager::executeCommand(const QString &command, const QStringList &arguments) {
    QProcess process;
    process.start(command, arguments);
    
    if (!process.waitForFinished(-1)) {
        errorMessage = QString("命令执行失败: %1").arg(process.errorString());
        logger->error(errorMessage);
        return false;
    }
    
    if (process.exitCode() != 0) {
        QString output = QString::fromUtf8(process.readAllStandardError());
        errorMessage = QString("命令执行失败，退出码: %1\n%2").arg(process.exitCode()).arg(output);
        logger->error(errorMessage);
        return false;
    }
    
    logger->info("命令执行成功");
    return true;
}

QString PackageManager::getPackageNameFromPath(const QString &packagePath) {
    QFileInfo fileInfo(packagePath);
    return fileInfo.baseName();
}
