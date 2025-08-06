#pragma once

#include <QObject>
#include <gen/gen_export.h>
#include <QProcess>

class GENLIB_EXPORT ExecuteCommandAsync : public QObject
{
    Q_OBJECT
public:
    ExecuteCommandAsync(QObject *parent = nullptr);

    void setTimeout(quint32 timeout);
    void execute(const QString &command, const QStringList &args = {}, quint32 timeout = 30000);

private slots:

signals:
    void outputReady(const QString &output);
    void resultAcquired(const QString &str);
    void finished(int result);
};

class ExecuteCommandThread : public QObject
{
    Q_OBJECT
public:
    ExecuteCommandThread(QObject *parent = nullptr);
    void setCommand(const QString &cmd, const QStringList &args = {});
    void setTimeout(quint32 timeout);

private:
    QString m_command;
    QStringList m_args;
    QProcess *m_process;
    quint32 m_timeout;

public slots:
    void run();

signals:
    void outputReady(const QString &output);
    void resultAcquired(const QString &str);
    void finished(int result);

private slots:
    void readyRead();
};
