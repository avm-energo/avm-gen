#include <QFileInfo>
#include <avm-gen/errorqueue.h>
#include <avm-gen/messagehandler.h>
#include <avm-gen/stdfunc.h>

QMutex MessageHandler::m_mutex = QMutex();
Logger MessageHandler::s_log;
Logger::LogLevels MessageHandler::s_queueLevel = Logger::LogLevels::LOGLEVEL_WARN;

void MessageHandler::messageHandlerWithErrorQueue(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&m_mutex);
    if (Logger::qtMessageTypeToLoglevel(type) <= s_queueLevel)
    {
        const QString sourceFile = QFileInfo(QString(context.file)).fileName();

        std::string function = context.function ? context.function : "";
        std::string rubbish(" __cdecl");
        StdFunc::RemoveSubstr(function, rubbish);

        ErrorMsg tmpm {
            QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"), // DateTime
            type,                                                         // Msg type
            sourceFile,                                                   // File
            context.line,                                                 // Line
            msg                                                           // Message
        };
        ErrorQueue::GetInstance().pushError(tmpm);
    }
    Logger::MessageTypes msgType = Logger::qtMessageTypeToMessageType(type);
    s_log.writeLog(msgType, msg);
}

void MessageHandler::setMessageHandlerFilename(const QString &filename)
{
    s_log.writeStart(filename);
}

void MessageHandler::setRawMessageHandlerFilename(const QString &filename)
{
    s_log.writeRawStart(filename);
}

void MessageHandler::setLogLevel(Logger::LogLevels level)
{
    s_log.setLogLevel(level);
}

void MessageHandler::setQueueLevel(Logger::LogLevels level)
{
    s_queueLevel = level;
}

void MessageHandler::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    Logger::MessageTypes msgType = Logger::qtMessageTypeToMessageType(type);
    QMutexLocker locker(&m_mutex);
    s_log.writeLog(msgType, msg);
}
