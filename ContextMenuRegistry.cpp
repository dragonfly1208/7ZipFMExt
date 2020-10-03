// ZipRegistry.cpp

#include "StdAfx.h"

//#include "../../../Common/IntToString.h"
//
//#include "../../../Windows/FileDir.h"
#include "Registry.h"
//#include "../../../Windows/Synchronization.h"

#include "ContextMenuRegistry.h"

using namespace NWindows;
using namespace NRegistry;


class CCriticalSection
{
	::CRITICAL_SECTION _object;
public:
	CCriticalSection() { InitializeCriticalSection(&_object); }
	~CCriticalSection() { DeleteCriticalSection(&_object); }
	void Enter() { EnterCriticalSection(&_object); }
	void Leave() { LeaveCriticalSection(&_object); }
};

class CCriticalSectionLock
{
	CCriticalSection *_object;
	void Unlock() { _object->Leave(); }
public:
	CCriticalSectionLock(CCriticalSection &object) : _object(&object) { _object->Enter(); }
	~CCriticalSectionLock() { Unlock(); }
};


static CCriticalSection g_CS;
#define CS_LOCK CCriticalSectionLock lock(g_CS);

#define APPNAME TEXT("7ZipFM")

static LPCTSTR const kCuPrefix = TEXT("Software") TEXT(STRING_PATH_SEPARATOR) APPNAME TEXT(STRING_PATH_SEPARATOR);

static CString GetKeyPath(LPCTSTR path) { return CString(kCuPrefix) + path; }

static LONG OpenMainKey(CKey &key, LPCTSTR keyName)
{
  return key.Open(HKEY_CURRENT_USER, GetKeyPath(keyName), KEY_READ);
}

static LONG CreateMainKey(CKey &key, LPCTSTR keyName)
{
  return key.Create(HKEY_CURRENT_USER, GetKeyPath(keyName));
}

static void Key_Set_BoolPair(CKey &key, LPCTSTR name, const CBoolPair &b)
{
  if (b.Def)
    key.SetValue(name, b.Val);
}

static void Key_Get_BoolPair(CKey &key, LPCTSTR name, CBoolPair &b)
{
  b.Val = false;
  b.Def = (key.GetValue_IfOk(name, b.Val) == ERROR_SUCCESS);
}

static void Key_Get_BoolPair_true(CKey &key, LPCTSTR name, CBoolPair &b)
{
  b.Val = true;
  b.Def = (key.GetValue_IfOk(name, b.Val) == ERROR_SUCCESS);
}

static LPCTSTR const kOptionsInfoKeyName = TEXT("Options");


static LPCTSTR const kCascadedMenu = TEXT("CascadedMenu");
static LPCTSTR const kContextMenu = TEXT("ContextMenu");
static LPCTSTR const kMenuIcons = TEXT("MenuIcons");
static LPCTSTR const kElimDup = TEXT("ElimDupExtract");

void CContextMenuInfo::Save() const
{
  CS_LOCK
  CKey key;
  CreateMainKey(key, kOptionsInfoKeyName);
  
  Key_Set_BoolPair(key, kCascadedMenu, Cascaded);
  Key_Set_BoolPair(key, kMenuIcons, MenuIcons);
  Key_Set_BoolPair(key, kElimDup, ElimDup);
  
  if (Flags_Def)
    key.SetValue(kContextMenu, Flags);
}

void CContextMenuInfo::Load()
{
  Cascaded.Val = true;
  Cascaded.Def = false;

  MenuIcons.Val = true;
  MenuIcons.Def = false;

  ElimDup.Val = true;
  ElimDup.Def = false;

  Flags = (UINT32)(INT32)-1;
  Flags_Def = false;
  
  CS_LOCK
  
  CKey key;
  if (OpenMainKey(key, kOptionsInfoKeyName) != ERROR_SUCCESS)
    return;
  
  Key_Get_BoolPair_true(key, kCascadedMenu, Cascaded);
  Key_Get_BoolPair_true(key, kElimDup, ElimDup);
  Key_Get_BoolPair(key, kMenuIcons, MenuIcons);

  Flags_Def = (key.GetValue_IfOk(kContextMenu, Flags) == ERROR_SUCCESS);
}


CString getAppPath() {
	CKey key;
	CString val;


	LONG ret = key.Open(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\")APPNAME _T(".exe"), KEY_READ);
	if (ret != ERROR_SUCCESS) {
		ret = key.Open(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\App Paths\\")APPNAME _T(".exe"), KEY_READ);
	}
	if (ret != ERROR_SUCCESS) 
		return val;
	key.QueryValue(_T(""), val);

	return val;

}
