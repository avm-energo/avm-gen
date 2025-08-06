#include "gen/executecommand.h"

#include <QDebug>
#include <QThread>

ExecuteCommandAsync::ExecuteCommandAsync(QObject *parent) : QObject(parent)
{
}

void ExecuteCommandAsync::setTimeout(quint32 timeout) {}

void ExecuteCommandAsync::execute(const QString &command, const QStringList &args, quint32 timeout)
{
    ExecuteCommandThread *threadObj = new ExecuteCommandThread;
    threadObj->setCommand(command, args);
    threadObj->setTimeout(timeout);
    auto execThread = new QThread;
    QObject::connect(execThread, &QThread::started, threadObj, &ExecuteCommandThread::run);
    QObject::connect(threadObj, &ExecuteCommandThread::resultAcquired, this, &ExecuteCommandAsync::resultAcquired);
    QObject::connect(threadObj,
                     &ExecuteCommandThread::outputReady,
                     this,
                     &ExecuteCommandAsync::outputReady);
    QObject::
        connect(threadObj, &ExecuteCommandThread::finished, this, &ExecuteCommandAsync::finished);
    QObject::connect(threadObj, &ExecuteCommandThread::finished, execThread, &QThread::exit);
    QObject::connect(execThread, &QThread::finished, &QObject::deleteLater);
    QObject::connect(execThread, &QThread::finished, threadObj, &QObject::deleteLater);
    threadObj->moveToThread(execThread);
    execThread->start();
}

ExecuteCommandThread::ExecuteCommandThread(QObject *parent) : QObject(parent)
{
}

void ExecuteCommandThread::setCommand(const QString &cmd, const QStringList &args)
{
    m_command = cmd;
    m_args = args;
}

void ExecuteCommandThread::setTimeout(quint32 timeout)
{
    m_timeout = timeout;
}

void ExecuteCommandThread::run()
{
    std::string result = "";

#if(SYSTEM_TYPE==0) // linux
    char buffer[128];
    FILE *pipe = popen(command.toLatin1(), "r");
    if (!pipe)
    {
        qDebug() << "pipe open failed";
        emit finished(-1);
    }
    //    qDebug() << "pipe opened";
    try
    {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            //            qDebug() << "++buffer: " << buffer;
            result += buffer;
        }
    } catch (...)
    {
        pclose(pipe);
        emit finished(-2);
    }
    //    qDebug() << "result is: " << result.c_str();
    emit finished(pclose(pipe));
#elif(SYSTEM_TYPE==1) // windows
    m_process = new QProcess(this);
    QMetaObject::Connection conn = connect(m_process,
                                           &QProcess::readyReadStandardOutput,
                                           this,
                                           &ExecuteCommandThread::readyRead);
    m_process->start(m_command, m_args);
    m_process->waitForFinished(m_timeout);
    disconnect(conn);
    delete m_process;
    result = "OK";
#else
#endif
    emit resultAcquired(result.c_str());
}

void ExecuteCommandThread::readyRead()
{
    if (m_process != nullptr)
        emit outputReady(m_process->readAllStandardOutput());
}
