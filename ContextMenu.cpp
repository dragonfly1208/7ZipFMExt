// ContextMenu.cpp : CZipContextMenu 的实现

#include "stdafx.h"
#include "ContextMenu.h"
#include "ContextMenuFlags.h"
#include "MyFileTool.h"
#include "ContextMenuRegistry.h"
#include <afx.h>
#include <afxwin.h>
#include <winuser.h>

// CZipContextMenu
#ifdef _WIN32
extern HINSTANCE g_hInstance;
#endif

static LPCSTR const kMainVerb = "SevenZip";
static LPCSTR const kOpenCascadedVerb = "SevenZip.OpenWithType.";
static LPCSTR const kCheckSumCascadedVerb = "SevenZip.Checksum";


struct CContextMenuCommand
{
	UINT32 flag;
	CZipContextMenu::ECommandInternalID CommandInternalID;
	LPCSTR Verb;
	CString ResourceID;
};

static const CContextMenuCommand g_Commands[] =
{
	{
		NContextMenuFlags::kNull,
		CZipContextMenu::kCommandNULL,
		"",
		_T("")
	},
	{
		NContextMenuFlags::kOpen,
		CZipContextMenu::kOpen,
		"Open",
		_T("打开压缩包")
	},
	{
		NContextMenuFlags::kExtract,
		CZipContextMenu::kExtract,
		"Extract",
		_T("提取文件...")
	},
	{
		NContextMenuFlags::kExtractHere,
		CZipContextMenu::kExtractHere,
		"ExtractHere",
		_T("提取到当前位置")
	},
	{
		NContextMenuFlags::kExtractTo,
		CZipContextMenu::kExtractTo,
		"ExtractTo",
		_T("提取到 \"%s\\\"")
	},
	{
		NContextMenuFlags::kTest,
		CZipContextMenu::kTest,
		"Test",
		_T("测试压缩包")
	},
	{
		NContextMenuFlags::kCompress,
		CZipContextMenu::kCompress,
		"Compress",
		_T("添加到压缩包...")
	},
	{
		NContextMenuFlags::kCompressEmail,
		CZipContextMenu::kCompressEmail,
		"CompressEmail",
		_T("压缩并邮寄...")
	},
	{
		NContextMenuFlags::kCompressTo7z,
		CZipContextMenu::kCompressTo7z,
		"CompressTo7z",
		_T("添加到 \"%s.7z\"")
	},
	{
		NContextMenuFlags::kCompressTo7zEmail,
		CZipContextMenu::kCompressTo7zEmail,
		"CompressTo7zEmail",
		_T("压缩 \"%s.7z\" 并邮寄")
	},
	{
		NContextMenuFlags::kCompressToZip,
		CZipContextMenu::kCompressToZip,
		"CompressToZip",
		_T("添加到 \"%s.zip\"")	
	},
	{
		NContextMenuFlags::kCompressToZipEmail,
		CZipContextMenu::kCompressToZipEmail,
		"CompressToZipEmail",
		_T("压缩 \"%s.zip\" 并邮寄")
	}
};

static  CString getGCmdsResrcID(CZipContextMenu::ECommandInternalID item, const CString& fname) {

	switch (UINT32(item))
	{
	case CZipContextMenu::kExtractTo:
	case CZipContextMenu::kCompressTo7z:
	case CZipContextMenu::kCompressTo7zEmail:
	case CZipContextMenu::kCompressToZip:
	case CZipContextMenu::kCompressToZipEmail: {
		CString ret;
		ret.Format(g_Commands[item].ResourceID, fname);
		return ret;
	}
	default:
		break;
	}
	return g_Commands[item].ResourceID;
}

//------------------------------------

//-----------------------------------------------------------------------------
static const CString  kExtractExludeExtensions =
" 3gp"
" aac ans ape asc asm asp aspx avi awk"
" bas bat bmp"
" c cs cls clw cmd cpp csproj css ctl cxx"
" def dep dlg dsp dsw"
" eps"
" f f77 f90 f95 fla flac frm"
" gif"
" h hpp hta htm html hxx"
" ico idl inc ini inl"
" java jpeg jpg js"
" la lnk log"
" mak manifest wmv mov mp3 mp4 mpe mpeg mpg m4a"
" ofr ogg"
" pac pas pdf php php3 php4 php5 phptml pl pm png ps py pyo"
" ra rb rc reg rka rm rtf"
" sed sh shn shtml sln sql srt swa"
" tcl tex tiff tta txt"
" vb vcproj vbs"
" wav wma wv"
" xml xsd xsl xslt"
" ";

/*
static const char * const kNoOpenAsExtensions =
" 7z arj bz2 cab chm cpio flv gz lha lzh lzma rar swm tar tbz2 tgz wim xar xz z zip ";
*/

static const char * const kOpenTypes[] =
{
	""
	, "*"
	, "#"
	, "#:e"
	// , "#:a"
	, "7z"
	, "zip"
	, "cab"
	, "rar"
};

struct CHashCommand
{
	CZipContextMenu::ECommandInternalID CommandInternalID;
	PCTSTR UserName;
	PCTSTR MethodName;
};

static const CHashCommand g_HashCommands[] =
{
	{ CZipContextMenu::kHash_CRC32,  _T("CRC-32"),  _T("CRC32") },
	{ CZipContextMenu::kHash_CRC64,  _T("CRC-64"),  _T("CRC64") },
	{ CZipContextMenu::kHash_SHA1,   _T("SHA-1"),   _T("SHA1") },
	{ CZipContextMenu::kHash_SHA256, _T("SHA-256"), _T("SHA256") },
	{ CZipContextMenu::kHash_All,    _T("*"),       _T("*") }
};


static const CString kIncludeExtensions =
	"7z;apm;ar,a,deb,lib;arj;bz2,bzip2,tbz2,tbz;"
	"cab;chm,chi,chq,chw;hxs,hxi,hxr,hxq,hxw,lit;"
	"msi,msp,doc,xls,ppt;cpio;cramfs;dmg;elf;"
	"ext,ext2,ext3,ext4,img;fat,img;flv;gpt,mbr;"
	"gz,gzip,tgz,tpz,apk;hfs,hfsx;ihex;iso,img;"
	"lzh,lha;lzma;lzma86;macho;mbr;mslz;mub;"
	"nsis;ntfs,img;exe,dll,sys;obj;te;pmd;"
	"qcow,qcow2,qcow2c;rar,r00;rar,r00;rpm;001;"
	"squashfs;swf;swf;tar,ova;udf,iso,img;scap;"
	"uefif;vdi;vhd;vmdk;wim,swm,esd,ppkg;xar,pkg,xip;"
	"xz,txz;z,taz;zip,z01,zipx,jar,xpi,odt,ods,docx,xlsx,epub,ipa,apk,appx;";

static bool FindExt(const CString &ext) {
	if (ext.IsEmpty()) return false;
	if (kIncludeExtensions.Find(ext) >= 0)
		return true;
	return false;
}


static CString Get7zFmPath()
{
	static CString path;
	if (path.IsEmpty()) {
		path = getAppPath();

		if (!path.IsEmpty()) return path;

		TCHAR szDLLFolder[MAX_PATH + 1];
		GetModuleFileName(g_hInstance, szDLLFolder, MAX_PATH);
		MyFileTool dll(szDLLFolder);
		path = dll.path();
		path.Append(_T("\\7ZipFM.exe"));

	}


	return path;//_T("F:\\\\C++\\git\\build-7ZipFileManager-Desktop_Qt_5_9_8_MSVC2015_32bit-Debug\\debug\\7ZipFM.exe");
}




//------------------------------------------------------------------------------------------------------
CZipContextMenu::CZipContextMenu():m_hBitmap(NULL)
{
	//BOOL ok = m_bitmap.LoadBitmap(IDB_ZIPICON);
	//BOOL ok = m_bitmap.LoadBitmap(_T("F:\\C++\\git\\others\\7ZipFMExt\\icons\\zipicon.bmp"));
	//HBITMAP hBitmap = LoadBitmap(AfxGetInstanceHandle(), _T("IDB_ZIPICON"));
	m_hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP));

	//_bitmap = ::LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_MENU_LOGO));
	int n = 0;
	if (m_hBitmap == 0) {
		n = GetLastError();
	}
}

CZipContextMenu::~CZipContextMenu()
{
	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	m_hBitmap = NULL;
}



STDMETHODIMP CZipContextMenu::Initialize(
	LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;
	TCHAR     filePath[MAX_PATH + 2];
	TCHAR     szRoot[MAX_PATH];
	TCHAR     szFileSystemName[256];
	UINT      uFile, uNumFiles;

	// Look for CF_HDROP data in the data object.
	if (FAILED(pDataObj->GetData(&fmt, &stg)))
	{
		// Nope! Return an "invalid argument" error back to Explorer.
		return E_INVALIDARG;
	}

	// Get a pointer to the actual data.
	hDrop = (HDROP)GlobalLock(stg.hGlobal);
	// Make sure it worked.
	if (NULL == hDrop)
		return E_INVALIDARG;

	// Sanity check - make sure there is at least one filename.
	uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	HRESULT hr = S_OK;

	if (0 == uNumFiles)
	{
		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	////!!!!MFC初始化
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		//??_Module.LogEvent(_T("MFC初始化错误！"), EVENTLOG_ERROR_TYPE );
		return 1;
	}
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());


	m_dropPath.Empty();

	// Get the name of the directory where the files were dropped.
	if (SHGetPathFromIDList(pidlFolder, filePath)) {
		// Get the root of the target folder, and see if it's on an NTFS volume.
		lstrcpy(szRoot, filePath);
		PathStripToRoot(szRoot);

		if (!GetVolumeInformation(szRoot, NULL, 0, NULL, NULL, NULL,
			szFileSystemName, 256))
		{
			// Couldn't determine file system type.
			return E_FAIL;
		}

#ifndef NOT_ON_WIN2K
		if (0 != lstrcmpi(szFileSystemName, _T("ntfs")))
		{
			// The file system isn't NTFS, so it doesn't support hard links.
			return E_FAIL;
		}
#endif

		// Make sure the target dir ends in a backslash, to make later processing easier.
		//PathAddBackslash(filePath);
		m_dropPath = filePath;
	}else {
		m_dropPath.Empty();
	}
	

	// Check each dropped item.  If there are any directories present,
	// we have to bail out, since a directory can't be linked to.
	// We also have to check that the dropped items reside on the same
	// volume as the directory where they were dropped - hard links can only
	// be made on the same volume.
	for (uFile = 0; uFile < uNumFiles; uFile++)
	{
		if (DragQueryFile(hDrop, uFile, filePath, MAX_PATH))
		{
			//if (PathIsDirectory(filePath))
			//{
			//	// We found a directory!  Bail out.
			//	m_lsDroppedFiles.RemoveAll();
			//	break;
			//}

			//if (!PathIsSameRoot(szFile, m_szFolderDroppedIn))
			//{
			//	// Dropped files came from a different volume - bail out.
			//	m_lsDroppedFiles.RemoveAll();
			//	break;
			//}

			// Quote the name if it contains spaces (needed so the cmd line is built properly)
			//PathQuoteSpaces(m_szFile);
			// Add the file to our list of dropped files.
			m_fileNames.Add(filePath);
		}else {
			m_fileNames.RemoveAll();
			hr = E_INVALIDARG;
		}
	}   // end for


	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);

	return hr;
}
#define MYINSERTSUBMENU(hSubMenu, uPosition, uIDNewItem, cmdId, ItemName)\
if(CascadedMenu){\
if (!MyInsertSubMenu(hSubMenu, uPosition, uIDNewItem, cmdId, ItemName)) { subPosition--; uID--; }\
}else{\
if (g_Commands[cmdId].flag & m_contextMenuFlags) \
	InsertMenuItemInfo2(hmenu, uMenuIndex++, uID++, cmdId, getGCmdsResrcID(cmdId, ItemName).GetBuffer());\
}



bool CZipContextMenu::MyInsertSubMenu(HMENU hSubMenu, UINT uPosition, UINT_PTR uIDNewItem, CZipContextMenu::ECommandInternalID cmdId, const CString &ItemName) {
	if (!(g_Commands[cmdId].flag & m_contextMenuFlags)) { return false; }
	InsertMenu(hSubMenu, uPosition, MF_BYPOSITION, uIDNewItem, getGCmdsResrcID(cmdId, ItemName));
	if (NULL != m_hBitmap)
		SetMenuItemBitmaps(hSubMenu, uPosition, MF_BYPOSITION, m_hBitmap, NULL);
	m_ECmdIntIDs.Add(cmdId);
	return true;
}
void CZipContextMenu::InsertMenuItemInfo2(HMENU hmenu, UINT uMenuIndex, UINT uID, CZipContextMenu::ECommandInternalID cmdId, PTSTR dwTypeData) {
	// Insert the submenu into the ctx menu provided by Explorer.
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = uID;
	mii.hSubMenu = 0;
	mii.dwTypeData = dwTypeData;
	InsertMenuItem(hmenu, uMenuIndex, TRUE, &mii);
	if (NULL != m_hBitmap)
		SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_hBitmap, NULL);
	m_ECmdIntIDs.Add(cmdId);
}


void CZipContextMenu::InsertMenuItemInfo(HMENU hmenu, UINT uMenuIndex, UINT uID, HMENU hSubmenu, PTSTR dwTypeData) {
	// Insert the submenu into the ctx menu provided by Explorer.
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = uID;
	mii.hSubMenu = hSubmenu;
	mii.dwTypeData = dwTypeData;
	InsertMenuItem(hmenu, uMenuIndex, TRUE, &mii);
	if (NULL != m_hBitmap)
		SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_hBitmap, NULL);
}

STDMETHODIMP CZipContextMenu::QueryContextMenu(
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags)
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	if (m_fileNames.GetSize() <= 0) return E_FAIL;

	CContextMenuInfo ci;
	ci.Load();

	if (!ci.MenuIcons.Val) {
		if(m_hBitmap)
			DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	
	bool CascadedMenu = ci.Cascaded.Val;

	m_contextMenuFlags = ci.Flags;

	bool isCrcHash = ci.Flags & NContextMenuFlags::kCRC;




	bool isMulti = false;
	bool isAllZip = true;
	bool isAllFile = true;
	//CString m_nameWithoutSuffix;
	//CString pDirName;
	//m_nameWithoutSuffix.GetBuffer();
	m_nameWithoutSuffix.Empty();
	isMulti = m_fileNames.GetSize()>1;

	for (int i = 0; i < m_fileNames.GetSize(); ++i) {
		MyFileTool fileTool(m_fileNames[i]);
		if (fileTool.isDir()) {
			isAllFile = false;
			isAllZip = false;
			m_nameWithoutSuffix = fileTool.fileName();
			break;
		}

		CString ext = fileTool.suffix().Trim().MakeLower();
		if (i == 0) {
			m_nameWithoutSuffix = ext.IsEmpty()? fileTool.fileName():fileTool.completeBaseName();
		}

		if (!FindExt(ext)) {
			isAllZip = false; break;
		}
	}
	if (isMulti) {
		MyFileTool fileTool(m_fileNames[0]);
		MyFileTool pFileTool(fileTool.path());
		if (pFileTool.isRoot()) {
			m_nameWithoutSuffix = pFileTool.filePath().GetAt(0);
		}else {
			m_nameWithoutSuffix = pFileTool.fileName();
		}
		
	}

	HMENU hSubmenu = CreatePopupMenu();
	UINT uID = uidFirstCmd;
	UINT subPosition = 0;
	// First, create and populate a submenu.
	if (isAllZip) {
		if (!isMulti) {
			MYINSERTSUBMENU(hSubmenu, subPosition++, uID++, CZipContextMenu::kOpen,CString());
		}
		MYINSERTSUBMENU(hSubmenu, subPosition++, uID++, CZipContextMenu::kExtract, CString());
		MYINSERTSUBMENU(hSubmenu, subPosition++, uID++, CZipContextMenu::kExtractHere, CString());

		MYINSERTSUBMENU(hSubmenu, subPosition++, uID++, CZipContextMenu::kExtractTo, isMulti?_T("*"):m_nameWithoutSuffix);

		MYINSERTSUBMENU(hSubmenu, subPosition++, uID++, CZipContextMenu::kTest, CString());
	}

	MYINSERTSUBMENU(hSubmenu, subPosition++,uID++, CZipContextMenu::kCompress, CString());
	//if (m_dropPath.IsEmpty())MyInsertSubMenu(hSubmenu, subPosition++,uID++, CZipContextMenu::kCompressEmail);
	MYINSERTSUBMENU(hSubmenu, subPosition++,uID++, CZipContextMenu::kCompressTo7z, m_nameWithoutSuffix);
	//MyInsertSubMenu(hSubmenu, subPosition++,uID++, CZipContextMenu::kCompressTo7zEmail, m_nameWithoutSuffix);
	MYINSERTSUBMENU(hSubmenu, subPosition++,uID++, CZipContextMenu::kCompressToZip, m_nameWithoutSuffix);
	//MyInsertSubMenu(hSubmenu, subPosition++,uID++, CZipContextMenu::kCompressToZipEmail, m_nameWithoutSuffix);

	
	if (isAllFile && isCrcHash) {
		HMENU hHashSubmenu = CreatePopupMenu();
		subPosition = 0;

		for (int i = 0; i < sizeof(g_HashCommands) / sizeof(CHashCommand); i++) {
			const CHashCommand &hashCmd = g_HashCommands[i];
			//MyInsertSubMenu(hHashSubmenu, subPosition++, uID++, hashCmd.CommandInternalID, hashCmd.MethodName);

			InsertMenu(hHashSubmenu, subPosition++, MF_BYPOSITION, uID++, hashCmd.UserName);
			m_ECmdIntIDs.Add(CZipContextMenu::ECommandInternalID(hashCmd.CommandInternalID));
			if (NULL != m_hBitmap)
				SetMenuItemBitmaps(hHashSubmenu, subPosition-1, MF_BYPOSITION, m_hBitmap, NULL);

		}
		// Insert the submenu into the ctx menu provided by Explorer.
		MENUITEMINFO miiHash = { sizeof(MENUITEMINFO) };
		if(CascadedMenu)
			InsertMenuItemInfo(hmenu, uMenuIndex++, uID++, hSubmenu, _T("7ZipFM"));
		if(isCrcHash)
			InsertMenuItemInfo(hmenu, uMenuIndex++, uID++, hHashSubmenu, _T("7ZipFM-Hash"));
	}else {
		if (CascadedMenu)
			InsertMenuItemInfo(hmenu, uMenuIndex++, uID++, hSubmenu, _T("7ZipFM"));
	}

	
	

	/*
	// First, create and populate a submenu.
	HMENU hSubmenu = CreatePopupMenu();
	UINT uID = uidFirstCmd;

	InsertMenu(hmenu, uMenuIndex++, MF_BYPOSITION, uID++, _T("SimpleShlExt Test Item"));

	InsertMenu(hSubmenu, 0, MF_BYPOSITION, uID++, _T("&Notepad"));
	InsertMenu(hSubmenu, 1, MF_BYPOSITION, uID++, _T("&Internet Explorer"));

	// Insert the submenu into the ctx menu provided by Explorer.
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = uID++;
	mii.hSubMenu = hSubmenu;
	mii.dwTypeData = _T("C&P Open With");

	InsertMenuItem(hmenu, uMenuIndex++, TRUE, &mii);
	*/
	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, uID - uidFirstCmd);
}

STDMETHODIMP CZipContextMenu::GetCommandString(
	UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	USES_CONVERSION;

	// Check idCmd, it must be 0 since we have only one menu item.
	if (idCmd > 2)
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if (uFlags & GCS_HELPTEXT)
	{
		LPCTSTR szText = _T("This is the simple shell extension's help");
		LPCTSTR szNotepadText = _T("Open the selected file in Notepad");
		LPCTSTR szIEText = _T("Open the selected file in Internet Explorer");
		LPCTSTR pszText = _T("1234");
		switch (idCmd)
		{
		case 0:
			pszText = szText;
			break;
		case 1:
			pszText = szNotepadText;
			break;
		case 2:
			pszText = szIEText;
			break;
		}
		if (uFlags & GCS_UNICODE)
		{
			// We need to cast pszName to a Unicode string, and then use the
			// Unicode string copy API.
			lstrcpynW((LPWSTR)pszName, T2CW(pszText), cchMax);
		}
		else
		{
			// Use the ANSI string copy API to return the help string.
			lstrcpynA(pszName, T2CA(pszText), cchMax);
		}

		return S_OK;
	}

	return E_INVALIDARG;
}
static CString GetQuotedString(const CString &s)
{
	CString s2(" \"");
	s2 += s;
	s2 += "\" ";
	return s2;
}
static CString GetQuotedPathsParam(const CArray<CString, const CString&> &fileNames)
{
	CString params;
	for (int i = 0; i < fileNames.GetSize(); ++i) {
		params += " -f ";
		CString fname = fileNames[i];
		params += GetQuotedString(fname);
	}
	return params;
}

STDMETHODIMP CZipContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if (0 != HIWORD(pCmdInfo->lpVerb) && LOWORD(pCmdInfo->lpVerb)>=m_ECmdIntIDs.GetSize())
		return E_INVALIDARG;
	// Get the command index - the only valid one is 0.
	ECommandInternalID cmdID = m_ECmdIntIDs[LOWORD(pCmdInfo->lpVerb)];

	CString params;
	if (!m_dropPath.IsEmpty() && cmdID != kOpen && cmdID != kTest) {
		params += "-dd ";
		params += GetQuotedString(m_dropPath);
	}
	switch (cmdID)
	{
	case kOpen:
	{
		params.Empty();
		params += GetQuotedString(m_fileNames[0]);

		//MyCreateProcess(Get7zFmPath(), params);
		ShellExecute(pCmdInfo->hwnd, _T("open"), Get7zFmPath(),
			params, NULL, SW_SHOW);
		break;
	}
	case kExtract:
	case kExtractHere:
	case kExtractTo:
	{
		if (cmdID == kExtractHere) {
			params += " -e";
		}else {
			params += " -x";
			params += (cmdID == kExtract) ?" -xd" :" -xdf";
		}
		params += GetQuotedPathsParam(m_fileNames);
		ShellExecute(pCmdInfo->hwnd, _T("open"), Get7zFmPath(),
			params, NULL, SW_SHOW);
		break;
	}
	case kTest:
	{
		params += " -T";
		params += GetQuotedPathsParam(m_fileNames);
		ShellExecute(pCmdInfo->hwnd, _T("open"), Get7zFmPath(),
			params, NULL, SW_SHOW);
		break;
	}case kCompress:
	case kCompressEmail:
	case kCompressTo7z:
	case kCompressTo7zEmail:
	case kCompressToZip:
	case kCompressToZipEmail:
	{
		params += " -a ";
		params += GetQuotedPathsParam(m_fileNames);

		//bool email =
		//	(cmdID == kCompressEmail) ||
		//	(cmdID == kCompressTo7zEmail) ||
		//	(cmdID == kCompressToZipEmail);
		if ((cmdID == kCompress) || (cmdID == kCompressEmail)) {//showDialog
			params += " -ad ";
			}
		if ((cmdID == kCompressTo7z) || (cmdID == kCompressTo7zEmail)) {
			if(pCmdInfo->lpDirectory && !m_nameWithoutSuffix.IsEmpty()){
				params.AppendFormat(_T(" -arc \"%s\\%s.7z\""), CString(pCmdInfo->lpDirectory), m_nameWithoutSuffix);
			}
			params += " -t 7z";
		}else if ((cmdID == kCompressToZip) || (cmdID == kCompressToZipEmail)) {
			if (pCmdInfo->lpDirectory && !m_nameWithoutSuffix.IsEmpty()) {
				params.AppendFormat(_T(" -arc \"%s\\%s.zip\""), CString(pCmdInfo->lpDirectory), m_nameWithoutSuffix);
			}
			params += " -t zip";
		}
		//bool addExtension = (cmdID == kCompress || cmdID == kCompressEmail);

		ShellExecute(pCmdInfo->hwnd, _T("open"), Get7zFmPath(),
			params, NULL, SW_SHOW);

		break;
	}
	case kHash_CRC32:
	case kHash_CRC64:
	case kHash_SHA1:
	case kHash_SHA256:
	case kHash_All:
	{
		params += " -ch ";
		params += "-t ";
		params += g_HashCommands[cmdID- kHash_CRC32].MethodName;
		params += GetQuotedPathsParam(m_fileNames);
		ShellExecute(pCmdInfo->hwnd, _T("open"), Get7zFmPath(),
			params, NULL, SW_SHOW);
		break;
	}
	default:
		return E_INVALIDARG;
		break;
	}
	return S_OK;
}

bool CZipContextMenu::InitMFC() {
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());    // init MFC
	////!!!!MFC初始化
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		//??_Module.LogEvent(_T("MFC初始化错误！"), EVENTLOG_ERROR_TYPE );
		return false;
	}
	return true;
}

STDMETHODIMP CZipContextMenu::Load(LPCOLESTR wszFilename, DWORD dwMode)
{
	if (!InitMFC()) return 1;
	UNREFERENCED_PARAMETER(dwMode);
	
	// Let CString convert the filename to ANSI.
	m_sFilename = wszFilename;
	OutputDebugString(CString("CZipContextMenu-Load:")+m_sFilename+"\n");
	//AfxMessageBox(CString("CZipContextMenu::Load--")+ m_sFilename);
	return S_OK;
}


STDMETHODIMP CZipContextMenu::DragEnter(
	IDataObject* pDataObj, DWORD grfKeyState,
	POINTL pt, DWORD* pdwEffect)
{
	if (!InitMFC()) return 1;
	*pdwEffect = DROPEFFECT_COPY;
	return S_OK;
	/*
	COleDataObject dataobj;
	TCHAR          szItem[MAX_PATH];
	UINT           uNumFiles;
	HGLOBAL        hglobal;
	HDROP          hdrop;

	dataobj.Attach(pDataObj, FALSE); // attach to the IDataObject, don't auto-release it

									 // Read the list of items from the data object.  They're stored in HDROP
									 // form, so just get the HDROP handle and then use the drag 'n' drop APIs
									 // on it.
	hglobal = dataobj.GetGlobalData(CF_HDROP);

	if (NULL != hglobal)
	{
		hdrop = (HDROP)GlobalLock(hglobal);

		uNumFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

		for (UINT uFile = 0; uFile < uNumFiles; uFile++)
		{
			if (0 != DragQueryFile(hdrop, uFile, szItem, MAX_PATH))
				m_lsDroppedFiles.AddTail(szItem);
		}

		GlobalUnlock(hglobal);
	}

	if (m_lsDroppedFiles.GetCount() > 0)
	{
		*pdwEffect = DROPEFFECT_COPY;
		return S_OK;
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
		return E_INVALIDARG;
	}*/
}

STDMETHODIMP CZipContextMenu::Drop(
	IDataObject* pDataObj, DWORD grfKeyState,
	POINTL pt, DWORD* pdwEffect)
{
	if (!InitMFC()) return 1;

	//CSendToCloneDlg dlg(&m_lsDroppedFiles);
	//CActCtx ctx;

	//dlg.DoModal();
	COleDataObject dataobj;
	TCHAR          szItem[MAX_PATH];
	UINT           uNumFiles;
	HGLOBAL        hglobal;
	HDROP          hdrop;

	dataobj.Attach(pDataObj, FALSE); // attach to the IDataObject, don't auto-release it

									 // Read the list of items from the data object.  They're stored in HDROP
									 // form, so just get the HDROP handle and then use the drag 'n' drop APIs
									 // on it.
	hglobal = dataobj.GetGlobalData(CF_HDROP);
	CArray<CString, const CString&> droppedFiles;
	if (NULL != hglobal)
	{
		hdrop = (HDROP)GlobalLock(hglobal);

		uNumFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

		for (UINT uFile = 0; uFile < uNumFiles; uFile++)
		{
			if (0 != DragQueryFile(hdrop, uFile, szItem, MAX_PATH))
				droppedFiles.Add(szItem);

				//m_lsDroppedFiles.AddTail(szItem);
		}

		GlobalUnlock(hglobal);
	}

	if (droppedFiles.GetCount() > 0)
	{
		CString params;
		params += " -add -arc ";
		params += GetQuotedString(m_sFilename);
		params += GetQuotedPathsParam(droppedFiles);
		ShellExecute(NULL, _T("open"), Get7zFmPath(),
			params, NULL, SW_SHOW);

		*pdwEffect = DROPEFFECT_COPY;
		return S_OK;
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
		return E_INVALIDARG;
	}

}

/*
STDMETHODIMP CZipContextMenu::GetInfoTip(DWORD dwFlags, LPWSTR* ppwszTip)
{
	return E_NOTIMPL;/*
	if (!InitMFC()) return 1;

	USES_CONVERSION;
	LPMALLOC   pMalloc;
	CStdioFile file;
	DWORD      dwFileSize;
	CString    sFirstLine;
	BOOL       bReadLine;
	CString    sTooltip;

	UNREFERENCED_PARAMETER(dwFlags);

	// Try to open the file.
	if (!file.Open(m_sFilename, CFile::modeRead | CFile::shareDenyWrite))
		return E_FAIL;

	// Get an IMalloc interface from the shell.
	if (FAILED(SHGetMalloc(&pMalloc)))
		return E_FAIL;

	// Get the size of the file.
	dwFileSize = file.GetLength();

	// Read in the first line from the file.
	bReadLine = file.ReadString(sFirstLine);

	sTooltip.Format(_T("File size: %lu"), dwFileSize);

	// If we were able to read in the first line, add it to the tooltip.
	if (bReadLine)
	{
		sTooltip += _T("\n");
		sTooltip += sFirstLine;
	}

	// Allocate a buffer for Explorer.  Note that the must pass the string 
	// back as a Unicode string, so the string length is multiplied by the 
	// size of a Unicode character.
	ULONG cbText = (5 + sTooltip.GetLength()) * sizeof(wchar_t);

	*ppwszTip = (LPWSTR)pMalloc->Alloc(cbText);

	// Release the IMalloc interface now that we're done using it.
	pMalloc->Release();

	if (NULL == *ppwszTip)
		return E_OUTOFMEMORY;
	OutputDebugString(CString("CZipContextMenu-GetInfoTip\n"));
	// Use the Unicode string copy function to put the tooltip text in the buffer.
	//wcscpy_s(*ppwszTip, cbText, T2COLE(LPCTSTR(sTooltip)));
	wcscpy_s(*ppwszTip, sTooltip.GetLength() * sizeof(wchar_t), T2COLE(LPCTSTR(sTooltip)));

	return S_OK;*/
//}