#ifndef VECTOR_FILTERING_GLOBAL_H
#define VECTOR_FILTERING_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VECTORFILTERING_LIBRARY)  // This should be defined when building the library
#  define VECTORFILTERING_EXPORT Q_DECL_EXPORT
#else
#  define VECTORFILTERING_EXPORT Q_DECL_IMPORT
#endif

#endif // VECTOR_FILTERING_GLOBAL_H
