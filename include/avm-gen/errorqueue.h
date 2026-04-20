#pragma once

#include <avm-gen/error.h>
#include <avm-gen/gen_export.h>
#include <avm-gen/singleton.h>
#include <vector>

class GENLIB_EXPORT ErrorQueue : public QObject, public Singleton<ErrorQueue>
{
    Q_OBJECT
public:
    /*! \brief Constructs an empty ErrorQueue.
     *  \warning This class is not internally thread-safe.  The caller is responsible
     *           for external synchronisation (e.g., MessageHandler uses its own QMutex).
     */
    explicit ErrorQueue();

    /*! \brief Returns a read-only pointer to the internal message pool.
     *  \note The pointer is only valid as long as no push/pop occurs.  Treat it as a
     *        snapshot reference; do not store it across operations.
     */
    const std::vector<ErrorMsg> *errMsgPool();

    /*! \brief Returns and removes the oldest message from the pool.
     *  \pre The pool must not be empty; check errMsgPool()->size() before calling.
     */
    ErrorMsg popError();

    /*! \brief Appends \a msg to the pool, evicting the oldest entry when the pool is full.
     *  \details Emits errCounts() with the incremented new-error counter after each push.
     */
    void pushError(const ErrorMsg &msg);

    /// \brief Resets the new-error counter to zero (does not clear the pool itself).
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
