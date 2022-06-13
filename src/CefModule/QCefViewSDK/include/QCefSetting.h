#ifndef QCEFSETTINGS_H
#define QCEFSETTINGS_H
#pragma once

#pragma region qt_headers
#include <QtCore/qglobal.h>
#include <QString>
#include <QMetaType>
#pragma endregion qt_headers

#ifdef QCEFVIEW_LIB
	#define QCEFVIEW_EXPORT Q_DECL_EXPORT
#else
	#define QCEFVIEW_EXPORT Q_DECL_IMPORT
#endif

/*
 cmd line£º
	"off-screen-rendering-enabled"
	"multi-threaded-message-loop"
*/

//cmd line has priority over QCefSetting.
//If you want to change CefSettings, you must call QCefSetting::set***() before QCefSetting::initializeCef().
namespace QCefSetting
{
	/// <summary>
	/// 
	/// </summary>
	void QCEFVIEW_EXPORT initializeCef();

	/// <summary>
	/// 
	/// </summary>
	void QCEFVIEW_EXPORT uninitializeCef();

	/// <summary>
	/// 
	/// </summary>
	bool QCEFVIEW_EXPORT setWindowlessRenderingEnabled(bool);

	/// <summary>
	/// 
	/// </summary>
	bool QCEFVIEW_EXPORT setBrowserSubProcessPath(const QString& path);

	/// <summary>
	/// 
	/// </summary>
	bool QCEFVIEW_EXPORT setLogFilePath(const QString& path);

	/// <summary>
	/// 
	/// </summary>
	bool QCEFVIEW_EXPORT setResourceDirectoryPath(const QString& path);

	/// <summary>
	/// 
	/// </summary>
	bool QCEFVIEW_EXPORT setLocalesDirectoryPath(const QString& path);

	/// <summary>
	/// 
	/// </summary>
	bool QCEFVIEW_EXPORT setUserAgent(const QString& agent);

	/// <summary>
	/// 
	/// </summary>
	const QCEFVIEW_EXPORT bool windowlessRenderingEnabled();

	/// <summary>
	/// 
	/// </summary>
	const QCEFVIEW_EXPORT QString browserSubProcessPath();

	/// <summary>
	/// 
	/// </summary>
	const QCEFVIEW_EXPORT QString logFilePath();

	/// <summary>
	/// 
	/// </summary>
	const QCEFVIEW_EXPORT QString resourceDirectoryPath();

	/// <summary>
	/// 
	/// </summary>
	const QCEFVIEW_EXPORT QString localesDirectoryPath();

	/// <summary>
	/// 
	/// </summary>
	const QCEFVIEW_EXPORT QString userAgent();
};

#endif
