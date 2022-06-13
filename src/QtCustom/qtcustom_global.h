#pragma once

#include <QtCore/qglobal.h>
#ifndef BUILD_STATIC
# if defined(QTCUSTOM_LIB)
#  define QTCUSTOM_EXPORT Q_DECL_EXPORT
# else
#  define QTCUSTOM_EXPORT Q_DECL_IMPORT
# endif
#else
# define QTCUSTOM_EXPORT
#endif
