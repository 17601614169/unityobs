#ifndef QTCUSTOM_EXPORT_H
#define QTCUSTOM_EXPORT_H

#if defined(QTCUSTOM_LIB)
#define CUSTOM_EXPORT __declspec(dllexport)
#else
#define CUSTOM_EXPORT __declspec(dllimport)
#endif

CUSTOM_EXPORT void VedioDeviceNotEnabled(const char* id, const char *name, bool enable);

#endif // !QTCUSTOM_EXPORT_H
