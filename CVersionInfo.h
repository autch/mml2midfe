
#pragma once

class CVersionInfo
{
private:
  TCHAR m_szModuleFileName[MAX_PATH];
  PBYTE m_pbyVersionInfo;
  DWORD m_dwVersionInfoSize;
  PCHAR m_pszFileVersion;
  VS_FIXEDFILEINFO m_vsFileInfo;

  BOOL Get()
  {
    DWORD dwHandle = 0;
    m_dwVersionInfoSize = ::GetFileVersionInfoSize(m_szModuleFileName, &dwHandle);
    if(m_dwVersionInfoSize > 0)
    {
      m_pbyVersionInfo = new BYTE[m_dwVersionInfoSize];
      if(::GetFileVersionInfo(m_szModuleFileName, dwHandle, m_dwVersionInfoSize, m_pbyVersionInfo))
      {
        UINT dwLength;
        PBYTE pbyValue;
/*
        if(::VerQueryValue(m_pbyVersionInfo, "\\StringFileInfo\\041104b0\\FileVersion", (LPVOID*)&pbyValue, &dwLength))
        {
          m_pszFileVersion = new CHAR[dwLength];
          memcpy(m_pszFileVersion, pbyValue, dwLength);
          return TRUE;
        }
*/
        if(::VerQueryValue(m_pbyVersionInfo, "\\", (LPVOID*)&pbyValue, &dwLength))
        {
          memcpy(&m_vsFileInfo, pbyValue, dwLength);
          return TRUE;
        }
      }
      delete[] m_pbyVersionInfo;
      m_pbyVersionInfo = NULL;
    }
    return FALSE;
  }

public:
  CVersionInfo(LPSTR szModuleFileName = NULL)
  {
    m_pbyVersionInfo = NULL;
    m_pszFileVersion = NULL;
    m_dwVersionInfoSize = 0;
    if(szModuleFileName)
    {
      strcpy_s(m_szModuleFileName, sizeof m_szModuleFileName, szModuleFileName);
    }
    else
    {
      ::GetModuleFileName(NULL, m_szModuleFileName, MAX_PATH);
    }
    Get();
  }
  virtual ~CVersionInfo()
  {
    delete[] m_pbyVersionInfo;
    delete[] m_pszFileVersion;
  }

  LPCSTR GetFileVersionString() const { return m_pszFileVersion; }
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
