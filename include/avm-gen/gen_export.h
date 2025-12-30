#pragma once

#include <QtCore/qglobal.h>

#if defined(GEN_EXPORTS)
#define GENLIB_EXPORT Q_DECL_EXPORT
#elif defined(GEN_IMPORTS)
#define GENLIB_EXPORT Q_DECL_IMPORT
#else
#define GENLIB_EXPORT
#endif
