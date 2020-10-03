// 7ZipFMExt.cpp : DLL 导出的实现。

//
// 注意:  COM+ 1.0 信息: 
//      请记住运行 Microsoft Transaction Explorer 以安装组件。
//      默认情况下不进行注册。

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include <VersionHelpers.h>
#include "My7ZipFMExt_i.h"
#include "ContextMenu.h"

CComModule _Module;
HINSTANCE g_hInstance = 0;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_ZipContextMenu, CZipContextMenu)
END_OBJECT_MAP()

using namespace ATL;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = (HINSTANCE)hInstance;
		_Module.Init(ObjectMap, hInstance, &LIBID_My7ZipFMExtLib);
		DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
		_Module.Term();
	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow()
{
	return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer()
{
	// If we're on NT, add ourselves to the list of approved shell extensions.

	// Note that you should *NEVER* use the overload of CRegKey::SetValue with
	// 4 parameters.  It lets you set a value in one call, without having to 
	// call CRegKey::Open() first.  However, that version of SetValue() has a
	// bug in that it requests KEY_ALL_ACCESS to the key.  That will fail if the
	// user is not an administrator.  (The code should request KEY_WRITE, which
	// is all that's necessary.)

	if (IsWindowsXPOrGreater())//(0 == (GetVersion() & 0x80000000))
	{
		CRegKey reg;
		LONG    lRet;

		lRet = reg.Open(HKEY_LOCAL_MACHINE,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
			KEY_SET_VALUE);

		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;

		lRet = reg.SetStringValue(_T("{FF770C85-53FE-4CCB-B5A9-D4B0511C6B4F}"),
			_T("7ZipFM Shell Extension"));

		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;
	}

	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer()
{
	// If we're on NT, remove ourselves from the list of approved shell extensions.
	// Note that if we get an error along the way, I don't bail out since I want
	// to do the normal ATL unregistration stuff too.

	if (IsWindowsXPOrGreater())//(0 == (GetVersion() & 0x80000000))
	{
		CRegKey reg;
		LONG    lRet;

		lRet = reg.Open(HKEY_LOCAL_MACHINE,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
			KEY_SET_VALUE);

		if (ERROR_SUCCESS == lRet)
		{
			lRet = reg.DeleteValue(_T("{FF770C85-53FE-4CCB-B5A9-D4B0511C6B4F}"));
		}
	}

	return _Module.UnregisterServer(FALSE);
}
