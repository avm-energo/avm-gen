#include <gen/errorqueue.h>
#include <gen/messagehandler.h>
#include <gen/stdfunc.h>

const QMap<QtMsgType, Logger::MessageTypes> c_msgTypesTranslateMap { { QtDebugMsg, Logger::Debug },
    { QtInfoMsg, Logger::Info }, { QtWarningMsg, Logger::Warning }, { QtCriticalMsg, Logger::Critical },
    { QtFatalMsg, Logger::Fatal } };

QMutex MessageHandler::m_mutex = QMutex();
Logger MessageHandler::s_log;

void MessageHandler::messageHandlerWithErrorQueue(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Logger::MessageTypes msgType = c_msgTypesTranslateMap[type];
    QMutexLocker locker(&m_mutex);
    QStringList buffer = QString(context.file).split("\\");
    QString sourceFile = buffer.isEmpty() ? "" : buffer.takeLast();

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
    s_log.writeLog(msgType, msg);
}

void MessageHandler::setMessageHandlerFilename(const QString &filename)
{
    s_log.writeStart(filename);
}

void MessageHandler::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    Logger::MessageTypes msgType = c_msgTypesTranslateMap[type];
    QMutexLocker locker(&m_mutex);
    s_log.writeLog(msgType, msg);
}
