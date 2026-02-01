#ifndef DEPENDENCYANALYZER_H
#define DEPENDENCYANALYZER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <memory>

class Logger;

struct DependencyNode {
    QString name;
    bool installed;
    QStringList dependencies;
    int level;
};

class DependencyAnalyzer {
public:
    explicit DependencyAnalyzer(std::shared_ptr<Logger> logger);

    // Analyze dependencies and return installation order
    QStringList getInstallationOrder(const QStringList &packages,
                                     const QMap<QString, QStringList> &dependencies);
    
    // Check if package is installed on system
    bool isPackageInstalled(const QString &packageName);
    
    // Get all dependencies recursively
    QStringList getAllDependencies(const QString &package,
                                   const QMap<QString, QStringList> &dependencies);
    
    // Check for circular dependencies
    bool hasCyclicDependency(const QMap<QString, QStringList> &dependencies);
    
    // Build dependency tree
    QMap<QString, DependencyNode> buildDependencyTree(
        const QStringList &packages,
        const QMap<QString, QStringList> &dependencies);
    
    // Get error message
    QString getErrorMessage() const;

private:
    bool hasCyclicDependencyHelper(const QString &node,
                                   QSet<QString> &visited,
                                   QSet<QString> &recursionStack,
                                   const QMap<QString, QStringList> &dependencies);
    
    void topologicalSortHelper(const QString &node,
                               QSet<QString> &visited,
                               QStringList &result,
                               const QMap<QString, QStringList> &dependencies);

    std::shared_ptr<Logger> logger;
    QString errorMessage;
};

#endif // DEPENDENCYANALYZER_H
