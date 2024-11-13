#ifndef IMPULSE_NOISE_GLOBAL_H
#define IMPULSE_NOISE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IMPULSE_NOISE_LIBRARY)
#  define IMPULSE_NOISE_EXPORT Q_DECL_EXPORT
#else
#  define IMPULSE_NOISE_EXPORT Q_DECL_IMPORT
#endif

#endif
