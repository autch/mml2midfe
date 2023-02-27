#pragma once

class CVersionInfo
{
private:
	std::string m_fileVersion{};
	TCHAR m_szModuleFileName[MAX_PATH]{};
	VS_FIXEDFILEINFO m_vsFileInfo{};

	BOOL Get()
	{
		DWORD dwHandle = 0;
		const DWORD m_dwVersionInfoSize = ::GetFileVersionInfoSize(m_szModuleFileName, &dwHandle);
		if (m_dwVersionInfoSize > 0)
		{
			const std::unique_ptr<BYTE[]> m_pbyVersionInfo = std::make_unique<BYTE[]>(m_dwVersionInfoSize);
			if (::GetFileVersionInfo(m_szModuleFileName, dwHandle, m_dwVersionInfoSize, m_pbyVersionInfo.get()))
			{
				UINT dwLength;
				PBYTE pbyValue;
				if (::VerQueryValue(m_pbyVersionInfo.get(), _T(R"(\StringFileInfo\041104b0\FileVersion)"), reinterpret_cast<LPVOID*>(&pbyValue), &dwLength))
				{
					m_fileVersion = std::string(reinterpret_cast<char*>(pbyValue), dwLength);
				}
				if (::VerQueryValue(m_pbyVersionInfo.get(), _T("\\"), reinterpret_cast<LPVOID*>(&pbyValue), &dwLength))
				{
					memcpy(&m_vsFileInfo, pbyValue, dwLength);
					return TRUE;
				}
			}
		}
		return FALSE;
	}

public:
	explicit CVersionInfo(LPTSTR szModuleFileName = nullptr)
	{
		if (szModuleFileName)
		{
			_tcscpy_s(m_szModuleFileName, MAX_PATH, szModuleFileName);
		}
		else
		{
			::GetModuleFileName(nullptr, m_szModuleFileName, MAX_PATH);
		}
		Get();
	}
	virtual ~CVersionInfo() = default;

	LPCSTR GetFileVersionString() const { return m_fileVersion.c_str(); }
	const VS_FIXEDFILEINFO* GetFixedFileInfo() const { return &m_vsFileInfo; }
	int FormatFileVersion(LPSTR szDst, DWORD dwSize) const
	{
		return sprintf_s(szDst, dwSize, "%0d.%0d.%0d.%0d",
			(m_vsFileInfo.dwFileVersionMS >> 16) & 0xffff,
			m_vsFileInfo.dwFileVersionMS & 0xffff,
			(m_vsFileInfo.dwFileVersionLS >> 16) & 0xffff,
			m_vsFileInfo.dwFileVersionLS & 0xffff
		);
	}
};
