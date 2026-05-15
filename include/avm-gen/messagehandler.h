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
#include <avm-gen/gen_export.h>
#include <avm-gen/logger.h>

class GENLIB_EXPORT MessageHandler
{
public:
    MessageHandler() { };

    /*! \brief Minimal Qt message handler that only writes to the log file (no ErrorQueue).
     *  \details Thread-safe via m_mutex.
     *           Install with: qInstallMessageHandler(MessageHandler::messageHandler).
     */
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    /*! \brief Qt message handler that logs to file AND pushes qualifying messages to ErrorQueue.
     *  \details Strips MSVC "__cdecl" decorations from the function name.  Thread-safe via
     *           m_mutex.  Install with: qInstallMessageHandler(MessageHandler::messageHandlerWithErrorQueue).
     */
    static void messageHandlerWithErrorQueue(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    /*! \brief Opens the log file and writes the start banner.
     *  \param filename  Base filename (without directory); forwarded to Logger::writeStart().
     *  \details Call this once at application startup after initialising Settings.
     */
    static void setMessageHandlerFilename(const QString &filename);

    /*! \brief Opens the log file and writes the start banner.
     *  \param filename  Base filename (without directory); forwarded to Logger::writeRawStart().
     *  \details Call this once at application startup after initialising Settings.
     */
    static void setRawMessageHandlerFilename(const QString &filename);

    /// \brief Sets the minimum log level for file output.
    static void setLogLevel(Logger::LogLevels level);

    /// \brief Sets the minimum log level at which messages are also pushed to ErrorQueue.
    static void setQueueLevel(Logger::LogLevels level);

private:
    static Logger s_log;
    static Logger::LogLevels s_queueLevel;
    static QMutex m_mutex;
};
