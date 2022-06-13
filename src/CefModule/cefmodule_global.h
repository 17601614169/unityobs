#ifndef CEFMODULE_GLOBAL_H
#define CEFMODULE_GLOBAL_H
#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
#if defined(CEFMODULE_LIB)
#define CEFMODULE_EXPORT Q_DECL_EXPORT
#else
#define CEFMODULE_EXPORT Q_DECL_IMPORT
#endif
#else
#define CEFMODULE_EXPORT
#endif

#endif // !cefmodule_global_h





