#pragma once

#include <gen/error.h>
#include <gen/gen_export.h>
#include <gen/singleton.h>
#include <vector>

class GENLIB_EXPORT ErrorQueue : public QObject, public Singleton<ErrorQueue>
{
    Q_OBJECT
public:
    explicit GENLIB_EXPORT ErrorQueue(token);
    const std::vector<ErrorMsg> GENLIB_EXPORT *errMsgPool();
    ErrorMsg GENLIB_EXPORT popError();
    void GENLIB_EXPORT pushError(const ErrorMsg &msg);

    void GENLIB_EXPORT clearCounter();
    auto GENLIB_EXPORT counter() const
    {
        return m_newErrors;
    }
signals:
    void GENLIB_EXPORT errCounts(size_t size);

private:
    std::vector<ErrorMsg> m_errMsgPool;
    size_t m_newErrors;
};
