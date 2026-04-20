#include <avm-gen/threadpool.h>

ThreadPool::ThreadPool(QObject *parent) : QObject { parent }
{
    m_thrCount = 1;
}

ThreadPool::~ThreadPool()
{
    while (!m_threadMap.isEmpty())
    {
        int id = m_threadMap.keys().first();
        Thread thr = m_threadMap[id];
        if (thr.thread != nullptr)
        {
            thr.thread->quit();
            thr.thread->wait();
            delete thr.thread;
        }
        m_threadMap.remove(id);
    }
}

void ThreadPool::start(int id)
{
    m_threadMap[id].thread->start();
    m_threadMap[id].status = RUNNING;
}

void ThreadPool::finish(int id)
{
    m_threadMap[id].thread->quit();
    m_threadMap[id].thread->wait();
}

ThreadPool::ThreadStatus ThreadPool::status(int id)
{
    if (m_threadMap.contains(id))
        return m_threadMap[id].status;
    else
        return DELETED;
}
