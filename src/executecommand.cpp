#include "gen/executecommand.h"

#include <QDebug>
#include <QThread>

ExecuteCommandAsync::ExecuteCommandAsync(QObject *parent) : QObject(parent)
{
}

void ExecuteCommandAsync::execute(const QString &command)
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

void ExecuteCommandThread::setCommand(const QString &cmd)
{
    command = cmd;
}

void ExecuteCommandThread::run()
{
    char buffer[128];
    std::string result = "";
    FILE *pipe = popen(command.toLatin1(), "r");
    if (!pipe)
    {
        qDebug() << "pipe open failed";
        emit finished(-1);
    }
    qDebug() << "pipe opened";
    try
    {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            qDebug() << "++buffer: " << buffer;
            result += buffer;
        }
    } catch (...)
    {
        pclose(pipe);
        emit finished(-2);
    }
    qDebug() << "result is: " << result.c_str();
    emit finished(pclose(pipe));
    emit resultAcquired(result.c_str());
}
