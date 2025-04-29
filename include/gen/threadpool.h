#pragma once

#include <QMap>
#include <QObject>
#include <QThread>
#include <gen/gen_export.h>

/*
 * ThreadPool class to simplify running various work in different threads
 * each work is addressed by its thread ID
 * Usual method of using it:
 * 1. Connect finished() signal of threadpool to any slot that has an id in its parameter list
 *      m_threadPool = new ThreadPool;
 *      connect(m_threadPool, &ThreadPool::finished, someClassObject, &SomeClass::anotherThreadFinishedSlot);
 * 2. Create new thread object pointing to run() slot and finished() signal
 *      int id = m_threadPool->create(anyObject, &anyObject::runSlot, &anyObject::finishedSignal);
 * 3. Start new thread
 *      m_threadPool->start(id);
 *
 * Use finish(int id) to stop running thread
 * Use status(int id) to get status of the thread
 */

class GENLIB_EXPORT ThreadPool : public QObject
{
    Q_OBJECT
public:
    enum ThreadStatus
    {
        STOPPED,
        DELETED,
        RUNNING
    };

    struct Thread
    {
        QThread *thread;
        ThreadStatus status;
    };

    explicit ThreadPool(QObject *parent = nullptr);
    ~ThreadPool();

    template <typename Functor> int create(QObject *context, Functor &&runMethod, void (QObject::*finishSignal)())
    {
        int id = m_thrCount++;
        Thread thr;
        thr.status = STOPPED;
        thr.thread = new QThread;
        context->moveToThread(thr.thread);
        connect(thr.thread, &QThread::finished, context, &QObject::deleteLater);
        connect(context, finishSignal, thr.thread, &QThread::quit);
        connect(thr.thread, &QThread::finished, [&, id]() {
            emit finished(id);
            m_threadMap[id].status = STOPPED;
            m_threadMap.remove(id);
        });
        connect(thr.thread, &QThread::started, context, runMethod);
        m_threadMap[id] = thr;
        return id;
    }

    template <typename Object> int create(Object *receiver, void (Object::*runMethod)(), void (Object::*finishSignal)())
    {
        int id = m_thrCount++;
        Thread thr;
        thr.status = STOPPED;
        thr.thread = new QThread;
        receiver->moveToThread(thr.thread);
        connect(thr.thread, &QThread::finished, receiver, &QObject::deleteLater);
        connect(receiver, finishSignal, thr.thread, &QThread::quit);
        connect(thr.thread, &QThread::finished, [&, id]() {
            emit finished(id);
            m_threadMap[id].status = STOPPED;
            m_threadMap.remove(id);
        });
        connect(thr.thread, &QThread::started, receiver, runMethod);
        m_threadMap[id] = thr;
        return id;
    };
    void start(int id);
    void finish(int id);
    ThreadStatus status(int id);

signals:
    void finished(int id);

private:
    QMap<int, Thread> m_threadMap;
    int m_thrCount;
};
