#ifndef IMPULSE_NOISE2_GLOBAL_H
#define IMPULSE_NOISE2_GLOBAL_H

#include <QtCore/qglobal.h>

// Define the export/import macros for the library based on whether it's being built or used
#if defined(IMPULSE_NOISE_LIBRARY)
#  define IMPULSE_NOISE2_EXPORT Q_DECL_EXPORT
#else
#  define IMPULSE_NOISE2_EXPORT Q_DECL_IMPORT
#endif


#endif // IMPULSE_NOISE2_GLOBAL_H
