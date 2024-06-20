#pragma once

#include <QObject>

class ExecuteCommandAsync : public QObject
{
    Q_OBJECT
public:
    ExecuteCommandAsync(QObject *parent = nullptr);

    void execute(std::string &command);

private slots:

signals:
    void resultAcquired(std::string &str);
    void finished(int result);
};

class ExecuteCommandThread : public QObject
{
    Q_OBJECT
public:
    ExecuteCommandThread(QObject *parent = nullptr);
    void setCommand(std::string &cmd);

private:
    std::string command;

public slots:
    void run();

signals:
    void resultAcquired(std::string &str);
    void finished(int result);
};
