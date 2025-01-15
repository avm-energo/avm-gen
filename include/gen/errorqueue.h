#pragma once

#include <gen/error.h>
#include <gen/gen_export.h>
#include <gen/singleton.h>
#include <vector>

class GENLIB_EXPORT ErrorQueue : public QObject, public Singleton<ErrorQueue>
{
    Q_OBJECT
public:
    explicit ErrorQueue(token);
    const std::vector<ErrorMsg> *errMsgPool();
    ErrorMsg popError();
    void pushError(const ErrorMsg &msg);

    void clearCounter();
    auto counter() const
    {
        return m_newErrors;
    }
signals:
    void errCounts(size_t size);

private:
    std::vector<ErrorMsg> m_errMsgPool;
    size_t m_newErrors;
};
