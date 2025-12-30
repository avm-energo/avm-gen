#pragma once

#include <avm-gen/gen_export.h>

/*! \brief Basic class for singleton pattern designed classes.
 *
 *  ## See also:
 *  - [Modern C++ Singleton Template](https://codereview.stackexchange.com/questions/173929/modern-c-singleton-template)
 * \emoji smile_cat
 */
template <typename T> class Singleton
{
public:
    static T &GetInstance();
    // Singleton(Singleton const &) = delete;
    // void operator=(Singleton const &) = delete;
};

/// \brief Returns instance of singleton class.
template <typename T> T &Singleton<T>::GetInstance()
{
    static T instance;
    return instance;
}
