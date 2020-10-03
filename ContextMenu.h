// ContextMenu.h : CZipContextMenu 的声明

#pragma once
#include "resource.h"       // 主符号
#include <vector>


#include "My7ZipFMExt_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CZipContextMenu

class ATL_NO_VTABLE CZipContextMenu :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZipContextMenu, &CLSID_ZipContextMenu>,
	public IDispatchImpl<IZipContextMenu, &IID_IZipContextMenu, &LIBID_My7ZipFMExtLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellExtInit,
	public IContextMenu,
	public IPersistFile,
	public IDropTarget
	//public IQueryInfo
{
public:
	enum ECommandInternalID
	{
		kCommandNULL,
		kOpen,
		kExtract,
		kExtractHere,
		kExtractTo,
		kTest,
		kCompress,
		kCompressEmail,
		kCompressTo7z,
		kCompressTo7zEmail,
		kCompressToZip,
		kCompressToZipEmail,
		kHash_CRC32,
		kHash_CRC64,
		kHash_SHA1,
		kHash_SHA256,
		kHash_All
	};
	struct CCommandItem
	{
		ECommandInternalID CommandInternalID;
		CString Verb;
		CString HelpString;
		CString Folder;
		CString ArcName;
		CString ArcType;
	};
	CZipContextMenu();

	~CZipContextMenu();

DECLARE_REGISTRY_RESOURCEID(IDR_ZIPCONTEXTMENU)


BEGIN_COM_MAP(CZipContextMenu)
	COM_INTERFACE_ENTRY(IZipContextMenu)
	COM_INTERFACE_ENTRY(IDispatch)

	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)

	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IDropTarget)
	//COM_INTERFACE_ENTRY(IQueryInfo)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// IShellExtInit
	STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IContextMenu
	//STDMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
	STDMETHODIMP GetCommandString(UINT_PTR, UINT, UINT *, LPSTR, UINT);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
	STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);
	
	// IPersistFile
	STDMETHODIMP GetClassID(LPCLSID) { OutputDebugString(CString("CZipContextMenu-GetClassID\n")); return E_NOTIMPL; }
	STDMETHODIMP IsDirty() { OutputDebugString(CString("CZipContextMenu-IsDirty\n")); return E_NOTIMPL; }
	STDMETHODIMP Load(LPCOLESTR, DWORD);
	STDMETHODIMP Save(LPCOLESTR, BOOL) { OutputDebugString(CString("CZipContextMenu-Save\n")); return E_NOTIMPL; }
	STDMETHODIMP SaveCompleted(LPCOLESTR) { OutputDebugString(CString("CZipContextMenu-SaveCompleted\n")); return E_NOTIMPL; }
	STDMETHODIMP GetCurFile(LPOLESTR*) { OutputDebugString(CString("CZipContextMenu-GetCurFile\n")); return E_NOTIMPL; }
	
	// IDropTarget
	STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState,
		POINTL pt, DWORD* pdwEffect);

	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
	{
		*pdwEffect = DROPEFFECT_COPY;
		return S_OK;
//		return E_NOTIMPL;
	}

	STDMETHODIMP DragLeave()
	{
		return S_OK;
	}

	STDMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState,
		POINTL pt, DWORD* pdwEffect);
	/*
	// IQueryInfo
    STDMETHODIMP GetInfoFlags(DWORD*)     { return E_NOTIMPL; }
    STDMETHODIMP GetInfoTip(DWORD, LPWSTR*);
	*/
private:
	bool MyInsertSubMenu(HMENU hMenu, UINT uPosition, UINT_PTR uIDNewItem, CZipContextMenu::ECommandInternalID cmdId,  const CString &ItemName = CString());
	void InsertMenuItemInfo2(HMENU hmenu, UINT uMenuIndex, UINT uID, CZipContextMenu::ECommandInternalID cmdId, PTSTR dwTypeData);
	void InsertMenuItemInfo(HMENU hmenu, UINT uMenuIndex, UINT uID, HMENU hSubmenu, PTSTR dwTypeData);

	bool InitMFC();
protected:

	//CBitmap     m_bitmap;
	HBITMAP		m_hBitmap;
	CString     m_dropPath;
	CArray<CString, const CString&> m_fileNames;

	CArray<ECommandInternalID, ECommandInternalID&> m_ECmdIntIDs;
	//CArray<CCommandItem, CCommandItem&> m_cmds;

	CString m_nameWithoutSuffix;
	CString m_sFilename;
	//CStringList m_lsDroppedFiles;
	
	UINT32 m_contextMenuFlags;
};

OBJECT_ENTRY_AUTO(__uuidof(ZipContextMenu), CZipContextMenu)
