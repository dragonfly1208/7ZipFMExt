// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0602     // enable Win 2K features
#define WINVER 0x0602           // enable 98/2K features
#define _WIN32_IE 0x0602        // enable IE4+ features

#define _ATL_APARTMENT_THREADED

// ***** NOTE *****
// This extension requires Win 2K or later and only works on NTFS volumes.
// If you're using NT 4 or 9x, or don't have an NTFS volume available,
// uncomment the following line to have the extension change behavior so it shows
// message boxes instead of making hard links.  Then it will run on all OSes.
//#define NOT_ON_WIN2K

// MFC
#include <afxwin.h>
#include <afxdisp.h>
#include <afxole.h>

// ATL
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atlconv.h>

// Win32
#include <shlwapi.h>
#include <comdef.h>
#include <shlobj.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
