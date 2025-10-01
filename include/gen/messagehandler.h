/* Message handler static class to intercept qt messages and redirect them into log file
 *
 * Using:
 * MessageHandler::setMessageHandlerFilename("logger.log");
 * MessageHandler::setLogLevel(Logger::LogLevels::LOGLEVEL_WARN);
 * qInstallMessageHandler(&MessageHandler::messageHandlerWithErrorQueue); // if using with ErrorQueue
 * or
 * qInstallMessageHandler(&MessageHandler::messageHandler); // simple logging
 */

#pragma once

#include <QObject>
#include <gen/gen_export.h>
#include <gen/logger.h>

class GENLIB_EXPORT MessageHandler
{
public:
    MessageHandler() { };
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void messageHandlerWithErrorQueue(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void setMessageHandlerFilename(const QString &filename);
    static void setLogLevel(Logger::LogLevels level);
    static void setQueueLevel(Logger::LogLevels level);

private:
    static Logger s_log;
    static Logger::LogLevels s_queueLevel;
    static QMutex m_mutex;
};
