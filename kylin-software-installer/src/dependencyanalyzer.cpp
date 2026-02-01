#include "dependencyanalyzer.h"
#include "logger.h"

#include <QProcess>
#include <QSet>
#include <algorithm>

DependencyAnalyzer::DependencyAnalyzer(std::shared_ptr<Logger> logger)
    : logger(logger)
{
}

QStringList DependencyAnalyzer::getInstallationOrder(const QStringList &packages,
                                                      const QMap<QString, QStringList> &dependencies) {
    logger->info("开始分析安装顺序");
    
    // Check for circular dependencies
    if (hasCyclicDependency(dependencies)) {
        errorMessage = "检测到循环依赖";
        logger->error(errorMessage);
        return QStringList();
    }
    
    // Get all packages including dependencies
    QSet<QString> allPackages;
    for (const QString &pkg : packages) {
        allPackages.insert(pkg);
        QStringList deps = getAllDependencies(pkg, dependencies);
        for (const QString &dep : deps) {
            allPackages.insert(dep);
        }
    }
    
    // Topological sort
    QSet<QString> visited;
    QStringList result;
    for (const QString &pkg : allPackages) {
        if (!visited.contains(pkg)) {
            topologicalSortHelper(pkg, visited, result, dependencies);
        }
    }
    
    logger->info(QString("安装顺序: %1").arg(result.join(", ")));
    return result;
}

bool DependencyAnalyzer::isPackageInstalled(const QString &packageName) {
    // Try dpkg first (Debian/Ubuntu)
    QProcess process;
    process.start("dpkg", QStringList() << "-l" << packageName);
    if (process.waitForFinished()) {
        if (process.exitCode() == 0) {
            return true;
        }
    }
    
    // Try rpm (RedHat/CentOS)
    process.start("rpm", QStringList() << "-q" << packageName);
    if (process.waitForFinished()) {
        if (process.exitCode() == 0) {
            return true;
        }
    }
    
    return false;
}

QStringList DependencyAnalyzer::getAllDependencies(const QString &package,
                                                    const QMap<QString, QStringList> &dependencies) {
    QStringList result;
    QSet<QString> visited;
    
    std::function<void(const QString &)> dfs = [&](const QString &pkg) {
        if (visited.contains(pkg)) {
            return;
        }
        visited.insert(pkg);
        
        if (dependencies.contains(pkg)) {
            for (const QString &dep : dependencies.value(pkg)) {
                result.append(dep);
                dfs(dep);
            }
        }
    };
    
    dfs(package);
    return result;
}

bool DependencyAnalyzer::hasCyclicDependency(const QMap<QString, QStringList> &dependencies) {
    QSet<QString> visited;
    QSet<QString> recursionStack;
    
    for (const QString &node : dependencies.keys()) {
        if (!visited.contains(node)) {
            if (hasCyclicDependencyHelper(node, visited, recursionStack, dependencies)) {
                return true;
            }
        }
    }
    
    return false;
}

QMap<QString, DependencyNode> DependencyAnalyzer::buildDependencyTree(
    const QStringList &packages,
    const QMap<QString, QStringList> &dependencies) {
    
    QMap<QString, DependencyNode> tree;
    
    std::function<void(const QString &, int)> buildTree = [&](const QString &pkg, int level) {
        if (tree.contains(pkg)) {
            return;
        }
        
        DependencyNode node;
        node.name = pkg;
        node.installed = isPackageInstalled(pkg);
        node.dependencies = dependencies.value(pkg, QStringList());
        node.level = level;
        
        tree[pkg] = node;
        
        for (const QString &dep : node.dependencies) {
            buildTree(dep, level + 1);
        }
    };
    
    for (const QString &pkg : packages) {
        buildTree(pkg, 0);
    }
    
    logger->info(QString("依赖树构建完成，共 %1 个节点").arg(tree.size()));
    return tree;
}

QString DependencyAnalyzer::getErrorMessage() const {
    return errorMessage;
}

bool DependencyAnalyzer::hasCyclicDependencyHelper(const QString &node,
                                                    QSet<QString> &visited,
                                                    QSet<QString> &recursionStack,
                                                    const QMap<QString, QStringList> &dependencies) {
    visited.insert(node);
    recursionStack.insert(node);
    
    if (dependencies.contains(node)) {
        for (const QString &neighbor : dependencies.value(node)) {
            if (!visited.contains(neighbor)) {
                if (hasCyclicDependencyHelper(neighbor, visited, recursionStack, dependencies)) {
                    return true;
                }
            } else if (recursionStack.contains(neighbor)) {
                return true;
            }
        }
    }
    
    recursionStack.remove(node);
    return false;
}

void DependencyAnalyzer::topologicalSortHelper(const QString &node,
                                               QSet<QString> &visited,
                                               QStringList &result,
                                               const QMap<QString, QStringList> &dependencies) {
    visited.insert(node);
    
    if (dependencies.contains(node)) {
        for (const QString &neighbor : dependencies.value(node)) {
            if (!visited.contains(neighbor)) {
                topologicalSortHelper(neighbor, visited, result, dependencies);
            }
        }
    }
    
    result.append(node);
}
