
#pragma once

#define SZ_KEYNAME "Software\\Autch\\MML2MIDfe\\Preferences"
#define SZ_VALUE_USEEDITOR "UseMyEditor"
#define SZ_VALUE_USEPLAYER "UseMyPlayer"

#define SZ_VALUE_MYEDITOR "MyEditor"
#define SZ_VALUE_MYEDITOR_WITH_LINE "MyEditorWithLine"

#define SZ_VALUE_MYPLAYER "MyPlayer"
#define SZ_VALUE_MML2MID "MML2MID"

class CPreferences
{
private:
  bool m_bUseMyEditor, m_bUseMyPlayer;
  CString m_sMyEditor, m_sMyEditorWithLine, m_sMyPlayer;
  CString m_sMML2MID;

  void QueryStringValue(ATL::CRegKey& rk, LPCTSTR szKeyName, CString& s)
  {
    TCHAR szTemp[1024];
    ULONG nSize = sizeof szTemp;
    if(rk.QueryStringValue(szKeyName, szTemp, &nSize) == ERROR_SUCCESS)
      s = szTemp;
  }
  void QueryBoolValue(ATL::CRegKey& rk, LPCTSTR szKeyName, bool& r)
  {
    DWORD t = 0;
    if(rk.QueryDWORDValue(szKeyName, t) == ERROR_SUCCESS)
      r = t ? true : false;
  }
  CString GetMML2MIDCommandLine()
  {
    CString s;
    s.Format("%s", m_sMML2MID);
    return s;
  }
  CString GetMyEditorCommandLine(CString sMMLFile)
  {
    CString s = m_sMyEditor;
    s.Replace(_T("%f"), sMMLFile.GetBuffer(20));
    return s;
  }
  CString GetMyEditorCommandLine(CString sMMLFile, int nLine)
  {
    CString s = m_sMyEditorWithLine, st;
    st.Format("%d", nLine);
    s.Replace(_T("%f"), sMMLFile.GetBuffer(20));
    s.Replace(_T("%l"), st.GetBuffer(20));
    return s;
  }
  CString GetMyPlayerCommandLine(CString sSMFFile)
  {
    CString s = m_sMyPlayer;
    s.Replace(_T("%f"), sSMFFile.GetBuffer(20));
    return s;
  }
  void MyCreateProcess(CString& szCmdLine)
  {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    if(CreateProcess(NULL, szCmdLine.GetBuffer(255), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
      CloseHandle(pi.hThread);
      CloseHandle(pi.hProcess);
    }
  }
public:
  CPreferences() : m_bUseMyEditor(false), m_bUseMyPlayer(false),
                   m_sMyEditor(), m_sMyEditorWithLine(), m_sMyPlayer(),
                   m_sMML2MID()
  {
  }
  CPreferences(const CPreferences& p)
  {
    m_bUseMyEditor = p.m_bUseMyEditor;
    m_bUseMyPlayer = p.m_bUseMyPlayer;
    m_sMyEditor = p.m_sMyEditor;
    m_sMyEditorWithLine = p.m_sMyEditorWithLine;
    m_sMyPlayer = p.m_sMyPlayer;
    m_sMML2MID = p.m_sMML2MID;
  }
  void LoadFromRegistry()
  {
    ATL::CRegKey rkRoot;

    LONG lRet = rkRoot.Open(HKEY_CURRENT_USER, SZ_KEYNAME); 
    if(lRet == ERROR_SUCCESS)
    {
      QueryStringValue(rkRoot, SZ_VALUE_MML2MID, m_sMML2MID);
      QueryStringValue(rkRoot, SZ_VALUE_MYEDITOR, m_sMyEditor);
      QueryStringValue(rkRoot, SZ_VALUE_MYEDITOR_WITH_LINE, m_sMyEditorWithLine);
      QueryStringValue(rkRoot, SZ_VALUE_MYPLAYER, m_sMyPlayer);
      QueryBoolValue(rkRoot, SZ_VALUE_USEEDITOR, m_bUseMyEditor);
      QueryBoolValue(rkRoot, SZ_VALUE_USEPLAYER, m_bUseMyPlayer);
      rkRoot.Close();
    }
  }
  void SaveToRegistry()
  {
    ATL::CRegKey rkRoot;

    LONG lRet = rkRoot.Create(HKEY_CURRENT_USER, SZ_KEYNAME); 
    rkRoot.SetDWORDValue(SZ_VALUE_USEEDITOR, m_bUseMyEditor);
    rkRoot.SetDWORDValue(SZ_VALUE_USEPLAYER, m_bUseMyPlayer);
    rkRoot.SetStringValue(SZ_VALUE_MML2MID, m_sMML2MID.GetBuffer(32));
    rkRoot.SetStringValue(SZ_VALUE_MYEDITOR, m_sMyEditor.GetBuffer(32));
    rkRoot.SetStringValue(SZ_VALUE_MYEDITOR_WITH_LINE, m_sMyEditorWithLine.GetBuffer(32));
    rkRoot.SetStringValue(SZ_VALUE_MYPLAYER, m_sMyPlayer.GetBuffer(32));  
    rkRoot.Close();
  }
  void LoadDefaultSettings()
  {
    m_sMML2MID = "mml2mid.exe";
    m_sMyEditor = m_sMyEditorWithLine = "";
    m_sMyPlayer = "";
    m_bUseMyEditor = m_bUseMyPlayer = false;
  }
  void EditMMLFile(HWND hWnd, CString sMMLFile)
  {
    CString s = sMMLFile;
    if(GetUseMyEditor())
      s = GetMyEditorCommandLine(sMMLFile);
    MyCreateProcess(s);
  }
  void EditMMLFile(HWND hWnd, CString sMMLFile, int nLine)
  {
    CString s = sMMLFile;
    if(GetUseMyEditor())
    {
      s = GetMyEditorCommandLine(sMMLFile, nLine);
      MyCreateProcess(s);
    }
    else
      ShellExecute(hWnd, NULL, sMMLFile.GetBuffer(255), NULL, NULL, SW_SHOWDEFAULT);
  }
  void PlaySMFFile(HWND hWnd, CString sSMFFile)
  {
    CString s = sSMFFile;
    if(GetUseMyPlayer())
    {
      s = GetMyPlayerCommandLine(sSMFFile);
      MyCreateProcess(s);
    }
    else
      ShellExecute(hWnd, NULL, sSMFFile.GetBuffer(255), NULL, NULL, SW_SHOWDEFAULT);
  }

  CString GetMML2MIDSetting() const { return m_sMML2MID; }
  CString GetMyEditorSetting() const { return m_sMyEditor; }
  CString GetMyEditorWithLineSetting() const { return m_sMyEditorWithLine; }
  CString GetMyPlayerSetting() const { return m_sMyPlayer; }
  bool GetUseMyEditor() const { return m_bUseMyEditor; }
  bool GetUseMyPlayer() const { return m_bUseMyPlayer; }

  void SetUseMyEditor(bool b) { m_bUseMyEditor = b; }
  void SetUseMyPlayer(bool b) { m_bUseMyPlayer = b; }
  void SetMML2MIDSetting(CString& s) { m_sMML2MID = s; }
  void SetMyEditorSetting(CString& s) { m_sMyEditor = s; }
  void SetMyEditorWithLineSetting(CString& s) { m_sMyEditorWithLine = s; }
  void SetMyPlayerSetting(CString& s) { m_sMyPlayer = s; }
};

