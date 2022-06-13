#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QTENGINEMODULE_LIB)
#  define QTENGINEMODULE_EXPORT Q_DECL_EXPORT
# else
#  define QTENGINEMODULE_EXPORT Q_DECL_IMPORT
# endif
#else
# define QTENGINEMODULE_EXPORT
#endif
