#include "stdafx.h"
#include "Preferences.h"

void QueryStringValue(ATL::CRegKey& rk, LPCTSTR szKeyName, CString& s)
{
	TCHAR szTemp[1024];
	ULONG nSize = 1024;
	if (rk.QueryStringValue(szKeyName, szTemp, &nSize) == ERROR_SUCCESS)
		s = szTemp;
}
void QueryBoolValue(ATL::CRegKey& rk, LPCTSTR szKeyName, bool& r)
{
	DWORD t = 0;
	if (rk.QueryDWORDValue(szKeyName, t) == ERROR_SUCCESS)
		r = t ? true : false;
}

void MyCreateProcess(CString& szCmdLine)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	if (CreateProcess(nullptr, szCmdLine.GetBuffer(255), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		DWORD err = GetLastError();
		TCHAR szMessage[4096];
		CString msg(_T("プロセスを起動できませんでした。"));

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err, 0, szMessage, 4096, nullptr);

		msg += _T("\n\n");
		msg += szCmdLine.GetBuffer(255);
		msg += _T("\n\n");
		msg += szMessage;

		MessageBox(nullptr, msg.GetBuffer(255), _T("プロセス起動失敗"), MB_ICONEXCLAMATION);
	}
}


CString CPreferences::GetMML2MIDCommandLine() const
{
	CString s;
	s.Format(_T("%s"), m_sMML2MID);
	return s;
}

CString CPreferences::GetMyEditorCommandLine(CString sMMLFile, int nLine) const
{
	CString s = m_sMyEditorWithLine, st;
	st.Format(_T("%d"), nLine);
	s.Replace(_T("%f"), sMMLFile.GetBuffer(20));
	s.Replace(_T("%l"), st.GetBuffer(20));
	return s;
}

CString CPreferences::GetMyPlayerCommandLine(CString sSMFFile) const
{
	CString s = m_sMyPlayer;
	s.Replace(_T("%f"), sSMFFile.GetBuffer(20));
	return s;
}

void CPreferences::LoadFromRegistry()
{
	ATL::CRegKey rkRoot;

	LONG lRet = rkRoot.Open(HKEY_CURRENT_USER, SZ_KEYNAME);
	if (lRet == ERROR_SUCCESS)
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

void CPreferences::SaveToRegistry()
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

void CPreferences::LoadDefaultSettings()
{
	m_sMML2MID = "mml2mid.exe";
	m_sMyEditor = m_sMyEditorWithLine = "";
	m_sMyPlayer = "";
	m_bUseMyEditor = m_bUseMyPlayer = false;
}

void CPreferences::EditMMLFile(HWND hWnd, const CString& sMMLFile)
{
	CString s = sMMLFile;
	if (GetUseMyEditor())
		s = GetMyEditorCommandLine(sMMLFile);
	MyCreateProcess(s);
}

void CPreferences::EditMMLFile(HWND hWnd, CString sMMLFile, int nLine)
{
	CString s = sMMLFile;
	if (GetUseMyEditor())
	{
		s = GetMyEditorCommandLine(sMMLFile, nLine);
		MyCreateProcess(s);
	}
	else
		ShellExecute(hWnd, nullptr, sMMLFile.GetBuffer(255), nullptr, nullptr, SW_SHOWDEFAULT);
}

void CPreferences::PlaySMFFile(HWND hWnd, CString sSMFFile)
{
	CString s = sSMFFile;
	if (GetUseMyPlayer())
	{
		s = GetMyPlayerCommandLine(sSMFFile);
		MyCreateProcess(s);
	}
	else
		ShellExecute(hWnd, nullptr, sSMFFile.GetBuffer(255), nullptr, nullptr, SW_SHOWDEFAULT);
}
