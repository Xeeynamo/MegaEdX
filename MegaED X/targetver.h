#pragma once

#ifdef _WIN64
    #define WINVER			_WIN32_WINNT_WINXP
    #define _WIN32_WINNT	_WIN32_WINNT_WINXP 
    #define _WIN32_WINDOWS	_WIN32_WINNT_WINXP
    #define _WIN32_IE		_WIN32_IE_XPSP1
#else
    #define WINVER			0x0410 // Windows 95
    #define _WIN32_WINNT	_WIN32_WINNT_NT4 
    #define _WIN32_WINDOWS	_WIN32_WINNT_NT4
    #define _WIN32_IE		_WIN32_IE_IE30
#endif
