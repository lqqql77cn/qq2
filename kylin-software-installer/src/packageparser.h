#ifndef PACKAGEPARSER_H
#define PACKAGEPARSER_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <memory>

class Logger;

struct PackageInfo {
    QString id;
    QString name;
    QString version;
    qint64 size;
    QString filename;
    QString checksum;
};

struct PackageMetadata {
    QString version;
    QString timestamp;
    QString targetSystem;
    QString targetArchitecture;
    QList<PackageInfo> packages;
    qint64 totalSize;
};

class PackageParser {
public:
    explicit PackageParser(std::shared_ptr<Logger> logger);

    // Parse package archive
    bool parsePackage(const QString &packagePath);
    
    // Get parsed metadata
    PackageMetadata getMetadata() const;
    
    // Get dependencies
    QMap<QString, QStringList> getDependencies() const;
    
    // Extract package to directory
    bool extractPackage(const QString &packagePath, const QString &extractDir);
    
    // Get error message
    QString getErrorMessage() const;

private:
    bool parseMetadata(const QString &metadataPath);
    bool parseDependencies(const QString &dependenciesPath);
    bool extractArchive(const QString &archivePath, const QString &extractDir);

    std::shared_ptr<Logger> logger;
    PackageMetadata metadata;
    QMap<QString, QStringList> dependencies;
    QString errorMessage;
};

#endif // PACKAGEPARSER_H
