#include "executecommand.h"

#include <QThread>

ExecuteCommandAsync::ExecuteCommandAsync(QObject *parent) : QObject(parent)
{
}

void ExecuteCommandAsync::execute(std::string &command)
{
    ExecuteCommandThread *threadObj = new ExecuteCommandThread;
    threadObj->setCommand(command);
    auto execThread = new QThread;
    QObject::connect(execThread, &QThread::started, threadObj, &ExecuteCommandThread::run);
    QObject::connect(threadObj, &ExecuteCommandThread::resultAcquired, this, &ExecuteCommandAsync::resultAcquired);
    QObject::connect(threadObj, &ExecuteCommandThread::finished, this, &ExecuteCommandAsync::finished);
    QObject::connect(threadObj, &ExecuteCommandThread::finished, execThread, &QThread::exit);
    QObject::connect(execThread, &QThread::finished, &QObject::deleteLater);
    QObject::connect(execThread, &QThread::finished, threadObj, &QObject::deleteLater);
    threadObj->moveToThread(execThread);
    execThread->start();
}

ExecuteCommandThread::ExecuteCommandThread(QObject *parent) : QObject(parent)
{
}

void ExecuteCommandThread::setCommand(std::string &cmd)
{
    command = cmd;
}

void ExecuteCommandThread::run()
{
    char buffer[128];
    std::string result = "";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        emit finished(-1);
    }
    try
    {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            result += buffer;
        }
    } catch (...)
    {
        pclose(pipe);
        emit finished(-2);
    }
    emit finished(pclose(pipe));
    emit resultAcquired(result);
}
