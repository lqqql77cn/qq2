#include "logger.h"
#include <QStandardPaths>
#include <QDir>
#include <iostream>

Logger::Logger() {
    // Create log directory
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    
    // Open log file
    QString logFilePath = logDir + "/kylin-installer.log";
    logFile.setFileName(logFilePath);
    logFile.open(QIODevice::Append | QIODevice::Text);
}

Logger::~Logger() {
    if (logFile.isOpen()) {
        logFile.close();
    }
}

void Logger::debug(const QString &message) {
    log(LogLevel::Debug, message);
}

void Logger::info(const QString &message) {
    log(LogLevel::Info, message);
}

void Logger::warning(const QString &message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const QString &message) {
    log(LogLevel::Error, message);
}

QString Logger::getLogFilePath() const {
    return logFile.fileName();
}

QString Logger::getAllLogs() const {
    QFile file(logFile.fileName());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());
        file.close();
        return content;
    }
    return QString();
}

void Logger::clearLogs() {
    QMutexLocker locker(&mutex);
    if (logFile.isOpen()) {
        logFile.close();
    }
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    logFile.close();
    logFile.open(QIODevice::Append | QIODevice::Text);
}

void Logger::log(LogLevel level, const QString &message) {
    QMutexLocker locker(&mutex);
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2] %3\n").arg(timestamp, levelStr, message);
    
    // Write to file
    if (logFile.isOpen()) {
        QTextStream stream(&logFile);
        stream << logMessage;
        logFile.flush();
    }
    
    // Also print to console
    std::cout << logMessage.toStdString();
}

QString Logger::levelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}
