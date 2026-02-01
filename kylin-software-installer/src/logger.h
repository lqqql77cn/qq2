#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    Logger();
    ~Logger();

    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);

    QString getLogFilePath() const;
    QString getAllLogs() const;
    void clearLogs();

private:
    void log(LogLevel level, const QString &message);
    QString levelToString(LogLevel level) const;

    QFile logFile;
    QMutex mutex;
};

#endif // LOGGER_H
