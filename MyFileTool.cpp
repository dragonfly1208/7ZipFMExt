#include "stdafx.h"
#include "MyFileTool.h"

#define SEP _T('\\')

MyFileTool::MyFileTool()
	: m_lastSeparator(-1),
	m_firstDotInFileName(-1),
	m_lastDotInFileName(-1)
{
}
MyFileTool::MyFileTool(const CString&filePath) 
	:m_lastSeparator(-2),
	m_firstDotInFileName(-2),
	m_lastDotInFileName(0) 
{
	m_filePath = filePath;
	m_filePath.Replace(_T('/'), SEP);
}
MyFileTool::~MyFileTool()
{
}
void MyFileTool::resolveFilePath() const
{
	;
//	if (m_filePath.IsEmpty() /*&& !m_nativeFilePath.IsEmpty()*/) {

//		m_filePath = QDir::fromNativeSeparators(m_nativeFilePath);
//#ifdef _WIN32
//		if (m_filePath.CompareNoCase(_T("//?/UNC/")))
//			m_filePath = m_filePath.remove(2, 6);
//		if (m_filePath.startsWith(_T("//?/")))
//			m_filePath = m_filePath.remove(0, 4);
//#endif
//	}
}

CString MyFileTool::filePath() const
{
	resolveFilePath();
	return m_filePath;
}

CString MyFileTool::fileName() const
{
	findLastSeparator();
#if defined(_WIN32)
	if (m_lastSeparator == -1 && m_filePath.GetLength() >= 2 && m_filePath.GetAt(1) == _T(':'))
		return m_filePath.Mid(2);
#endif
	return m_filePath.Mid(m_lastSeparator + 1);
}


CString MyFileTool::path() const
{
	findLastSeparator();
	if (m_lastSeparator == -1) {
#if defined(_WIN32)
		if (m_filePath.GetLength() >= 2 && m_filePath.GetAt(1) == _T(':'))
			return m_filePath.Left(2);
#endif
		return CString(_T('.'));
	}
	if (m_lastSeparator == 0)
		return CString(_T('/'));
#if defined(_WIN32)
	if (m_lastSeparator == 2 && m_filePath.GetAt(1) == _T(':'))
		return m_filePath.Left(m_lastSeparator + 1);
#endif
	return m_filePath.Left(m_lastSeparator);
}

CString MyFileTool::baseName() const
{
	findFileNameSeparators();
	int length = -1;
	if (m_firstDotInFileName >= 0) {
		length = m_firstDotInFileName;
		if (m_lastSeparator != -1) // avoid off by one
			length--;
	}
#if defined(_WIN32)
	if (m_lastSeparator == -1 && m_filePath.GetLength() >= 2 && m_filePath.GetAt(1) == _T(':'))
		return m_filePath.Mid(2, length - 2);
#endif
	return m_filePath.Mid(m_lastSeparator + 1, length);
}

CString MyFileTool::completeBaseName() const
{
	findFileNameSeparators();
	int length = -1;
	if (m_firstDotInFileName >= 0) {
		length = m_firstDotInFileName + m_lastDotInFileName;
		if (m_lastSeparator != -1) // avoid off by one
			length--;
	}
#if defined(_WIN32)
	if (m_lastSeparator == -1 && m_filePath.GetLength() >= 2 && m_filePath.GetAt(1) == _T(':'))
		return m_filePath.Mid(2, length - 2);
#endif
	return m_filePath.Mid(m_lastSeparator + 1, length);
}

CString MyFileTool::suffix() const
{
	findFileNameSeparators();

	if (m_lastDotInFileName == -1)
		return CString();

	return m_filePath.Mid(__max((INT16)0, m_lastSeparator) + m_firstDotInFileName + m_lastDotInFileName + 1);
}

CString MyFileTool::completeSuffix() const
{
	findFileNameSeparators();
	if (m_firstDotInFileName == -1)
		return CString();

	return m_filePath.Mid(__max((INT16)0, m_lastSeparator) + m_firstDotInFileName + 1);
}

//VC++判断文件或文件夹是否存在
//https://www.cnblogs.com/2018shawn/articles/9849975.html
bool MyFileTool::exists() const{
	return exists(m_filePath);
}
bool MyFileTool::exists(const CString &file) {
	//DWORD dwAttrib = GetFileAttributes(file);
	//return INVALID_FILE_ATTRIBUTES != dwAttrib;

	WIN32_FILE_ATTRIBUTE_DATA attrs = { 0 };
	return 0 != GetFileAttributesEx(file, GetFileExInfoStandard, &attrs);
}
bool MyFileTool::isFile() const {
	DWORD dwAttrib = GetFileAttributes(m_filePath);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}
bool MyFileTool::isDir() const {
	DWORD dwAttrib = GetFileAttributes(m_filePath);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}


#if defined(_WIN32)
bool MyFileTool::isRelative() const
{
	resolveFilePath();
	return (m_filePath.IsEmpty()
		|| (m_filePath.GetAt(0) != '/'
			&& !(m_filePath.GetLength() >= 2 && m_filePath.GetAt(1) == ':')));
}

bool MyFileTool::isAbsolute() const
{
	resolveFilePath();
	return ((m_filePath.GetLength() >= 3
		&& isalpha(m_filePath.GetAt(0))//可能错误
		&& m_filePath.GetAt(1) == ':'
		&& m_filePath.GetAt(2) == '/')
		|| (m_filePath.GetLength() >= 2
			&& m_filePath.GetAt(0) == _T('/')
			&& m_filePath.GetAt(1) == _T('/')));
}
#else
bool MyFileTool::isRelative() const
{
	return !isAbsolute();
}

bool MyFileTool::isAbsolute() const
{
	resolveFilePath();
	return (!m_filePath.IsEmpty() && (m_filePath.GetAt(0).unicode() == '/'));
}
#endif

#if defined(_WIN32)
bool MyFileTool::isDriveRoot() const
{
	resolveFilePath();
	return MyFileTool::isDriveRootPath(m_filePath);
}

bool MyFileTool::isDriveRootPath(const CString &path)const
{
#ifndef Q_OS_WINRT
	return (path.GetLength() == 3
		&& isalpha(path.GetAt(0)) && path.GetAt(1) == _T(':')
		&& path.GetAt(2) == SEP);
#else // !Q_OS_WINRT
	return path == QDir::rootPath();
#endif // !Q_OS_WINRT
}
#endif // _WIN32

bool MyFileTool::isRootPath(const CString &path)const
{
	if (path == _T("/")
#if defined(_WIN32)
		|| isDriveRootPath(path)
		|| isUncRoot(path)
#endif
		)
		return true;

	return false;
}

bool MyFileTool::isRoot() const
{
	resolveFilePath();
	return isRootPath(m_filePath);
}

bool MyFileTool::isEmpty() const
{
	return m_filePath.IsEmpty() /*&& m_nativeFilePath.IsEmpty()*/;
}



// private methods

void MyFileTool::findLastSeparator() const
{
	if (m_lastSeparator == -2) {
		resolveFilePath();
		m_lastSeparator = m_filePath.ReverseFind(SEP);
	}
}


void MyFileTool::findFileNameSeparators() const
{
	if (m_firstDotInFileName == -2) {
		resolveFilePath();
		int firstDotInFileName = -1;
		int lastDotInFileName = -1;
		int lastSeparator = m_lastSeparator;

		int stop;
		if (lastSeparator < 0) {
			lastSeparator = -1;
			stop = 0;
		}
		else {
			stop = lastSeparator;
		}

		int i = m_filePath.GetLength() - 1;
		for (; i >= stop; --i) {
			if (m_filePath.GetAt(i)== _T('.')) {
				firstDotInFileName = lastDotInFileName = i;
				break;
			}
			else if (m_filePath.GetAt(i) == SEP) {
				lastSeparator = i;
				break;
			}
		}

		if (lastSeparator != i) {
			for (--i; i >= stop; --i) {
				if (m_filePath.GetAt(i) == _T('.'))
					firstDotInFileName = i;
				else if (m_filePath.GetAt(i) == SEP) {
					lastSeparator = i;
					break;
				}
			}
		}
		m_lastSeparator = lastSeparator;
		m_firstDotInFileName = firstDotInFileName == -1 ? -1 : firstDotInFileName - __max(0, lastSeparator);
		if (lastDotInFileName == -1)
			m_lastDotInFileName = -1;
		else if (firstDotInFileName == lastDotInFileName)
			m_lastDotInFileName = 0;
		else
			m_lastDotInFileName = lastDotInFileName - firstDotInFileName;
	}
}

//static func
#ifdef _WIN32

CString MyFileTool::toNativeSeparators(const CString &pathName) {
	CString pathName1 = pathName;
	pathName1.Replace(_T('/'), _T('\\'));
	return pathName1;
}
CString MyFileTool::fromNativeSeparators(const CString &pathName) {
	CString pathName1 = pathName;
	pathName1.Replace(_T('\\'), _T('/'));
	return pathName1;
}

bool MyFileTool::isUncRoot(const CString &server)
{
	CString localPath = MyFileTool::toNativeSeparators(server);
	if ( !(localPath.GetLength() >= 2 && localPath[0] == (_T('\\')) && localPath[1] == (_T('\\'))))
		return false;

	int idx = localPath.Find(_T('\\'), 2);
	if (idx == -1 || idx + 1 == localPath.GetLength())
		return true;

	return localPath.Right(localPath.GetLength() - idx - 1).Trim().IsEmpty();
}

//CString MyFileTool::fixIfRelativeUncPath(const CString &path)
//{
//	CString currentPath = QDir::currentPath();
//	if (currentPath.startsWith(QLatin1String("//")))
//		return currentPath % QChar(QLatin1Char('/')) % path;
//	return path;
//}
#else
CString MyFileTool::toNativeSeparators(const CString &pathName) {
	CString pathName1 = pathName;
	pathName1.Replace(_T('\\'), _T('/'));
	return pathName1;
}
CString MyFileTool::fromNativeSeparators(const CString &pathName) {
	CString pathName1 = pathName;
	pathName1.Replace(_T('\\'), _T('/'));
	return pathName1;
}
#endif // _WIN32