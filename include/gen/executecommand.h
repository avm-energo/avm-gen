#pragma once

#include <QObject>

class ExecuteCommandAsync : public QObject
{
    Q_OBJECT
public:
    ExecuteCommandAsync(QObject *parent = nullptr);

    void execute(const QString &command);

private slots:

signals:
    void resultAcquired(const QString &str);
    void finished(int result);
};

class ExecuteCommandThread : public QObject
{
    Q_OBJECT
public:
    ExecuteCommandThread(QObject *parent = nullptr);
    void setCommand(const QString &cmd);

private:
    QString command;

public slots:
    void run();

signals:
    void resultAcquired(const QString &str);
    void finished(int result);
};
