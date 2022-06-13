	fronted 添加项目
	QStomp
	QtCustom
	
	win-dshow
		c++->additional directories:
			D:\obsproj\studio\QtCustom
		link->input
			D:\obsproj\studio\build\UI\$(Configuration)\QtCustom.lib
	obs
		link->input
			D:\obsproj\studio\build\UI\$(Configuration)\QtCustom.lib
			D:\obsproj\studio\build\UI\$(Configuration)\QStomp.lib
		c++->additional directories:
			D:\obsproj\studio\QtCustom
			D:\obsproj\studio\stomp\src
			D:\obsproj\studio\include