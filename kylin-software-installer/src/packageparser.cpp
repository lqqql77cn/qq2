#include "packageparser.h"
#include "logger.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QDir>
#include <QTemporaryDir>

PackageParser::PackageParser(std::shared_ptr<Logger> logger)
    : logger(logger)
{
}

bool PackageParser::parsePackage(const QString &packagePath) {
    logger->info(QString("开始解析软件包: %1").arg(packagePath));
    
    // Create temporary directory for extraction
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        errorMessage = "无法创建临时目录";
        logger->error(errorMessage);
        return false;
    }
    
    // Extract archive
    if (!extractArchive(packagePath, tempDir.path())) {
        return false;
    }
    
    // Parse metadata
    QString metadataPath = tempDir.path() + "/metadata.json";
    if (!parseMetadata(metadataPath)) {
        return false;
    }
    
    // Parse dependencies
    QString dependenciesPath = tempDir.path() + "/dependencies.json";
    if (!parseDependencies(dependenciesPath)) {
        logger->warning("未找到依赖文件，将跳过依赖分析");
    }
    
    logger->info("软件包解析完成");
    return true;
}

PackageMetadata PackageParser::getMetadata() const {
    return metadata;
}

QMap<QString, QStringList> PackageParser::getDependencies() const {
    return dependencies;
}

bool PackageParser::extractPackage(const QString &packagePath, const QString &extractDir) {
    logger->info(QString("提取软件包到: %1").arg(extractDir));
    return extractArchive(packagePath, extractDir);
}

QString PackageParser::getErrorMessage() const {
    return errorMessage;
}

bool PackageParser::parseMetadata(const QString &metadataPath) {
    QFile metadataFile(metadataPath);
    if (!metadataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = QString("无法打开 metadata.json: %1").arg(metadataPath);
        logger->error(errorMessage);
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(metadataFile.readAll());
    metadataFile.close();
    
    if (!doc.isObject()) {
        errorMessage = "metadata.json 格式无效";
        logger->error(errorMessage);
        return false;
    }
    
    QJsonObject obj = doc.object();
    metadata.version = obj.value("version").toString();
    metadata.timestamp = obj.value("timestamp").toString();
    metadata.targetSystem = obj.value("targetSystem").toString();
    metadata.targetArchitecture = obj.value("targetArchitecture").toString();
    metadata.totalSize = obj.value("totalSize").toVariant().toLongLong();
    
    // Parse packages array
    QJsonArray packagesArray = obj.value("packages").toArray();
    for (const QJsonValue &value : packagesArray) {
        QJsonObject pkgObj = value.toObject();
        PackageInfo pkg;
        pkg.id = pkgObj.value("id").toString();
        pkg.name = pkgObj.value("name").toString();
        pkg.version = pkgObj.value("version").toString();
        pkg.size = pkgObj.value("size").toVariant().toLongLong();
        pkg.filename = pkgObj.value("filename").toString();
        pkg.checksum = pkgObj.value("checksum").toString();
        metadata.packages.append(pkg);
    }
    
    logger->info(QString("解析 metadata 成功，包含 %1 个软件包").arg(metadata.packages.size()));
    return true;
}

bool PackageParser::parseDependencies(const QString &dependenciesPath) {
    QFile dependenciesFile(dependenciesPath);
    if (!dependenciesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = QString("无法打开 dependencies.json: %1").arg(dependenciesPath);
        logger->warning(errorMessage);
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(dependenciesFile.readAll());
    dependenciesFile.close();
    
    if (!doc.isObject()) {
        errorMessage = "dependencies.json 格式无效";
        logger->warning(errorMessage);
        return false;
    }
    
    QJsonObject obj = doc.object();
    QJsonObject depsObj = obj.value("dependencies").toObject();
    
    for (const QString &key : depsObj.keys()) {
        QJsonArray depsArray = depsObj.value(key).toArray();
        QStringList depsList;
        for (const QJsonValue &value : depsArray) {
            depsList.append(value.toString());
        }
        dependencies[key] = depsList;
    }
    
    logger->info(QString("解析依赖关系成功，共 %1 个包").arg(dependencies.size()));
    return true;
}

bool PackageParser::extractArchive(const QString &archivePath, const QString &extractDir) {
    // Determine archive type
    QString archiveType;
    if (archivePath.endsWith(".tar.gz")) {
        archiveType = "tar.gz";
    } else if (archivePath.endsWith(".zip")) {
        archiveType = "zip";
    } else {
        errorMessage = "不支持的压缩格式";
        logger->error(errorMessage);
        return false;
    }
    
    // Extract using appropriate tool
    QProcess process;
    if (archiveType == "tar.gz") {
        process.start("tar", QStringList() << "-xzf" << archivePath << "-C" << extractDir);
    } else if (archiveType == "zip") {
        process.start("unzip", QStringList() << "-q" << archivePath << "-d" << extractDir);
    }
    
    if (!process.waitForFinished()) {
        errorMessage = QString("提取压缩包失败: %1").arg(process.errorString());
        logger->error(errorMessage);
        return false;
    }
    
    if (process.exitCode() != 0) {
        errorMessage = QString("提取压缩包失败，退出码: %1").arg(process.exitCode());
        logger->error(errorMessage);
        return false;
    }
    
    logger->info(QString("成功提取压缩包到: %1").arg(extractDir));
    return true;
}
