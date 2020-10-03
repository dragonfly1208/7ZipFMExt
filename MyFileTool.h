#pragma once
class MyFileTool
{
public:
	MyFileTool();
	MyFileTool(const CString&filePath);
	~MyFileTool();

	CString filePath() const;
	CString fileName() const;
	CString path() const;
	CString baseName() const;
	CString completeBaseName() const;
	CString suffix() const;
	CString completeSuffix() const;

	bool exists() const;
	static bool exists(const CString &file);
	bool isFile() const;
	bool isDir() const;

	bool isRelative() const;
	bool isAbsolute() const;
	bool isDriveRoot() const;
	bool isDriveRootPath(const CString &path)const;
	bool isRootPath(const CString &path)const;
	bool isRoot() const;
	bool isEmpty() const;

	static CString toNativeSeparators(const CString &pathName);
	static CString fromNativeSeparators(const CString &pathName);
private:
	void resolveFilePath() const;
	void findLastSeparator() const;
	void findFileNameSeparators() const;

	static bool isUncRoot(const CString &server);
	//static CString fixIfRelativeUncPath(const CString &path);
private:
	mutable CString m_filePath; // always has slashes as separator
	//mutable NativePath m_nativeFilePath; // native encoding and separators

	mutable INT16 m_lastSeparator; // index in m_filePath of last separator
	mutable INT16 m_firstDotInFileName; // index after m_filePath for first dot (.)
	mutable INT16 m_lastDotInFileName; // index after m_firstDotInFileName for last dot (.)
};

