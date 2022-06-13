#ifndef QSTOMP_GLOBAL_H
#define QSTOMP_GLOBAL_H

#include <QtCore/qglobal.h>

#define P_DECLARE_PRIVATE(Class) \
	inline Class##Private* pd_func() { return reinterpret_cast<Class##Private *>(this->pd_ptr); } \
	inline const Class##Private* pd_func() const { return reinterpret_cast<const Class##Private *>(this->pd_ptr); } \
	friend class Class##Private;

#define P_DECLARE_PUBLIC(Class) \
	inline Class* pq_func() { return static_cast<Class *>(this->pq_ptr); } \
	inline const Class* pq_func() const { return static_cast<const Class *>(this->pq_ptr); } \
	friend class Class;

#define P_D(Class) Class##Private * const d = this->pd_func()
#define P_Q(Class) Class * const q = this->pq_func()

#if defined(QSTOMP_LIBRARY)
#  define QSTOMP_SHARED_EXPORT Q_DECL_EXPORT
#  define QSTOMP_P_INCLUDE
#else
#  define QSTOMP_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QSTOMP_GLOBAL_H
