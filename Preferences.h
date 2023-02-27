#pragma once

constexpr auto SZ_KEYNAME = _T("Software\\Autch\\MML2MIDfe\\Preferences");
constexpr auto SZ_VALUE_USEEDITOR = _T("UseMyEditor");
constexpr auto SZ_VALUE_USEPLAYER = _T("UseMyPlayer");

constexpr auto SZ_VALUE_MYEDITOR = _T("MyEditor");
constexpr auto SZ_VALUE_MYEDITOR_WITH_LINE = _T("MyEditorWithLine");

constexpr auto SZ_VALUE_MYPLAYER = _T("MyPlayer");
constexpr auto SZ_VALUE_MML2MID = _T("MML2MID");


class CPreferences
{
private:
	bool m_bUseMyEditor, m_bUseMyPlayer;
	CString m_sMyEditor, m_sMyEditorWithLine, m_sMyPlayer;
	CString m_sMML2MID;

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
	CString GetMML2MIDSetting() const { return m_sMML2MID; }
	CString GetMyEditorSetting() const { return m_sMyEditor; }
	CString GetMyEditorWithLineSetting() const { return m_sMyEditorWithLine; }
	CString GetMyPlayerSetting() const { return m_sMyPlayer; }
	bool GetUseMyEditor() const { return m_bUseMyEditor; }
	bool GetUseMyPlayer() const { return m_bUseMyPlayer; }

	void SetUseMyEditor(bool b) { m_bUseMyEditor = b; }
	void SetUseMyPlayer(bool b) { m_bUseMyPlayer = b; }
	void SetMML2MIDSetting(const CString& s) { m_sMML2MID = s; }
	void SetMyEditorSetting(const CString& s) { m_sMyEditor = s; }
	void SetMyEditorWithLineSetting(const CString& s) { m_sMyEditorWithLine = s; }
	void SetMyPlayerSetting(const CString& s) { m_sMyPlayer = s; }


	CString GetMML2MIDCommandLine() const;
	CString GetMyEditorCommandLine(CString sMMLFile, int nLine = 1) const;
	CString GetMyPlayerCommandLine(CString sSMFFile) const;

	void LoadFromRegistry();
	void SaveToRegistry();
	void LoadDefaultSettings();
	void EditMMLFile(HWND hWnd, const CString& sMMLFile);
	void EditMMLFile(HWND hWnd, CString sMMLFile, int nLine);
	void PlaySMFFile(HWND hWnd, CString sSMFFile);
};

