#pragma once

#include <QObject>
#include <gen/logger.h>

class MessageHandler
{
public:
    MessageHandler() { };
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void messageHandlerWithErrorQueue(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void setMessageHandlerFilename(const QString &filename);

private:
    static Logger s_log;
    static QMutex m_mutex;
};
