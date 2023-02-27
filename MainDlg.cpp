// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "Preferences.h"
#include "aboutdlg.h"
#include "MainDlg.h"
#include "RunMML2MIDProcs.h"
#include "PrefDlg.h"
#include "CVersionInfo.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((!m_acc.IsNull() && m_acc.TranslateAccelerator(m_hWnd, pMsg)))
		return TRUE;

	return IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	// center the dialog on the screen
	CenterWindow();

	//GetModuleFileName(GetModuleHandle(NULL), m_szAppPath, MAX_PATH);
	//PathRemoveFileSpec(m_szAppPath);
	//SetCurrentDirectory(m_szAppPath);

	m_pref.LoadDefaultSettings();
	m_pref.LoadFromRegistry();

	// set icons
	auto hIcon = static_cast<HICON>(::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
	                                            IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON),
	                                            LR_DEFAULTCOLOR));
	SetIcon(hIcon, TRUE);
	auto hIconSmall = static_cast<HICON>(::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
	                                                 IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON),
	                                                 LR_DEFAULTCOLOR));
	SetIcon(hIconSmall, FALSE);

	m_acc.LoadAccelerators(IDR_MAINFRAME);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	m_edMMLFile = GetDlgItem(IDC_MMLFILEEDIT);
	m_edSMFFile = GetDlgItem(IDC_SMFFILEEDIT);
	m_edMessage = GetDlgItem(IDC_MSGEDIT);

	// トランスポーズ関係
	m_udTranspose = GetDlgItem(IDC_TRSANSPOSESPIN); m_udTranspose.SetRange(-12, 12);
	m_edTranspose = GetDlgItem(IDC_TRANSPOSEEDIT); m_edTranspose.SetWindowText(_T("0"));

	// デフォルトで Format 1 を使う
	UISetCheck(IDC_SMFFORMAT1RADIO, TRUE);

	// リストビューの準備
	m_lvMessages = GetDlgItem(IDC_MESSAGELIST);
	m_lvMessages.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	{
		CImageList il;
		il.Create(16, 16, ILC_COLOR8 | ILC_MASK, 2, 1);

		// ビットマップリソースをイメージリストに追加
		{ CBitmap bmp; bmp.LoadBitmap(IDB_INFO); il.Add(bmp, RGB(255, 0, 255)); }
		{ CBitmap bmp; bmp.LoadBitmap(IDB_WARNING); il.Add(bmp, RGB(255, 0, 255)); }
		{ CBitmap bmp; bmp.LoadBitmap(IDB_ERROR); il.Add(bmp, RGB(255, 0, 255)); }

		// イメージリストをリストビューコントロールに設定
		m_lvMessages.SetImageList(il, LVSIL_SMALL);
	}

	// リストビューにカラムを用意する
	{
		CRect rcList;
		m_lvMessages.GetWindowRect(rcList);
		int nScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
		int n3DEdge = GetSystemMetrics(SM_CXEDGE);
		m_lvMessages.InsertColumn(0, _T("メッセージ"), LVCFMT_LEFT,
			rcList.Width() - nScrollWidth - n3DEdge * 2, -1);
	}
	UIEnable(IDC_COMPILEBUTTON, FALSE);
	UIEnable(IDC_EDITBUTTON, FALSE);
	UIEnable(IDC_PLAYBUTTON, FALSE);

	m_mnuMenu = GetMenu();
	m_mnuMenu.EnableMenuItem(IDM_COMPILE, MF_GRAYED);
	m_mnuMenu.EnableMenuItem(IDM_PLAY, MF_GRAYED);
	m_mnuMenu.EnableMenuItem(IDM_EDIT, MF_GRAYED);

	CVersionInfo info(nullptr);
	CHAR szStringFileVersion[32];
	info.FormatFileVersion(szStringFileVersion, sizeof szStringFileVersion);
	CHAR szFormattedVersion[128];
	sprintf_s(szFormattedVersion, sizeof szFormattedVersion, "MML2MIDfe Version %s", szStringFileVersion);

	AddMessage(szFormattedVersion);

	// mml2mid のバージョン情報を取得
	CString s;
	if (GetMML2MIDBanner(m_pref.GetMML2MIDSetting(), s))
		AddMessage(s);
	else
		AddMessage(_T("ERROR! mml2mid.exe が見つかりませんでした．\r\n「設定」で正しいパスを指定してください．"));

	return TRUE;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	m_pref.SaveToRegistry();
	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnBnClickedBrowsemmlbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	CFileDialog fdOpenFile(TRUE, _T("*.mml"), nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("MML ファイル (*.mml)\0*.mml\0すべてのファイル\0*.*\0\0"));

	m_edMMLFile.GetWindowText(fdOpenFile.m_szFileName, MAX_PATH);
	if (fdOpenFile.DoModal() == IDOK)
	{
		m_edMMLFile.SetWindowText(fdOpenFile.m_szFileName);

		// smf ファイル名を自動生成
		TCHAR szText[1024];
		m_edMMLFile.GetWindowText(szText, 1024);
		CString src(szText);
		src = src.Left(src.ReverseFind('.')) + _T(".mid");
		m_edSMFFile.SetWindowText(src.GetBuffer(40));
	}

	return 0;
}

LRESULT CMainDlg::OnBnClickedBrowsesmfbutton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	CFileDialog fdOpenFile(FALSE, _T("*.mid"), nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("標準 MIDI ファイル (*.mid)\0*.mid\0すべてのファイル\0*.*\0\0"));

	m_edSMFFile.GetWindowText(fdOpenFile.m_szFileName, _MAX_PATH);
	if (fdOpenFile.DoModal() == IDOK)
	{
		m_edSMFFile.SetWindowText(fdOpenFile.m_szFileName);
	}

	return 0;
}

LRESULT CMainDlg::OnEnSetfocusFileedit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	GetDialogItem<CEdit>(wID).SetSelAll();

	return 0;
}

LRESULT CMainDlg::OnBnClickedCompilebutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	CString sStdOut, sStdErr;
	CString sCmdLine = MakeupCommandLine();

	m_lvMessages.DeleteAllItems();
	AddMessage(sCmdLine);

	RunMML2MID(sCmdLine, sStdOut, sStdErr);

	if (AddMessage(sStdOut))
	{
		UIEnable(IDC_PLAYBUTTON, TRUE);
		m_mnuMenu.EnableMenuItem(IDM_PLAY, MF_ENABLED);
	}
	m_mnuMenu.EnableMenuItem(IDM_EDIT, MF_ENABLED);
	UIEnable(IDC_EDITBUTTON, TRUE);
	return 0;
}

BOOL CMainDlg::AddMessage(CString s)
{
	BOOL f = TRUE;
	while (s.GetLength())
	{
		CString part;
		int p = s.Find(_T("\x0d\x0a"));
		if (p == -1)
		{
			part = s;
			s = _T("");
		}
		else
		{
			part = s.Left(p);
			s = s.Mid(p + 2);
		}
		int nIndex = DecideIcon(part);
		if (nIndex == 2) f = FALSE;
		m_lvMessages.AddItem(m_lvMessages.GetItemCount(), 0, part.GetBuffer(128), nIndex);
	}
	m_lvMessages.EnsureVisible(m_lvMessages.GetItemCount() - 1, FALSE);
	return f;
}

int CMainDlg::DecideIcon(CString& s)
{
	// メッセージ内容からアイコンを決める
	if (s.Find(_T("ERROR! ")) != -1)
		return 2;
	if (s.Find(_T("Warning: ")) != -1)
		return 1;

	return 0;
}

LRESULT CMainDlg::OnEnTransposeedit(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	if (m_udTranspose.m_hWnd != nullptr)
	{
		BOOL bError;
		m_udTranspose.GetPos(&bError);
		if (bError)
			m_udTranspose.SetPos(0);
	}
	return 0;
}

LRESULT CMainDlg::OnAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// TODO : ここにコマンド ハンドラ コードを追加します。
	//CSimpleDialog<IDD_ABOUTBOX> dlg;
	CAboutDlg dlg;
	dlg.DoModal();

	return 0;
}

LRESULT CMainDlg::OnBnClickedPlaybutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR szTemp[1024];
	m_edSMFFile.GetWindowText(szTemp, 1024);
	CString sCmdLine(szTemp);
	m_pref.PlaySMFFile(*this, sCmdLine);
	return 0;
}
LRESULT CMainDlg::OnBnClickedEditbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR szTemp[1024];
	m_edMMLFile.GetWindowText(szTemp, 1024);
	CString sCmdLine(szTemp);
	m_pref.EditMMLFile(*this, sCmdLine);
	return 0;
}

CString CMainDlg::MakeupCommandLine()
{
	CString sCmdLine = "", sTemp;
	TCHAR szTemp[1024];

	sCmdLine.Format(_T("\"%s\" "), m_pref.GetMML2MIDSetting());
	if (GetDialogItem<CButton>(IDC_INVERTOCTCHECK).GetCheck())
		sCmdLine += _T("-x ");
	if (GetDialogItem<CButton>(IDC_INVERTVOLCHECK).GetCheck())
		sCmdLine += _T("-v ");
	if (GetDialogItem<CButton>(IDC_SMFFORMAT0RADIO).GetCheck())
		sCmdLine += _T("-f ");
	GetDialogItem<CEdit>(IDC_TRANSPOSEEDIT).GetWindowText(szTemp, 1024);
	if (_tstoi(szTemp) != 0)
	{
		sTemp.Format(_T("-t%s "), szTemp);
		sCmdLine += sTemp;
	}

	CString sMMLFile, sSMFFile;

	m_edMMLFile.GetWindowText(szTemp, 1024); sMMLFile = szTemp;
	m_edSMFFile.GetWindowText(szTemp, 1024); sSMFFile = szTemp;

	sTemp.Format(_T(R"("%s" "%s")"), sMMLFile, sSMFFile);
	sCmdLine += sTemp;

	return sCmdLine;
}

LRESULT CMainDlg::OnEnFileedit(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	UINT f = (m_edMMLFile.GetWindowTextLength() > 0
		&& m_edSMFFile.GetWindowTextLength() > 0)
		? MF_ENABLED : MF_GRAYED;

	UIEnable(IDC_COMPILEBUTTON, (f == MF_ENABLED));

	m_mnuMenu.EnableMenuItem(IDM_COMPILE, f);

	return 0;
}

LRESULT CMainDlg::OnMML2MIDWeb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	ShellExecute(*this, nullptr, _T("http://hpc.jp/~mml2mid/"), nullptr, nullptr, SW_SHOWDEFAULT);
	return 0;
}
LRESULT CMainDlg::OnAutchNet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	ShellExecute(*this, nullptr, _T("http://www.autch.net/"), nullptr, nullptr, SW_SHOWDEFAULT);
	return 0;
}
LRESULT CMainDlg::OnHelpContents(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// TODO : ヘルプファイルを作る
	ShellExecute(*this, nullptr, _T("mml2midfe.txt"), nullptr, nullptr, SW_SHOWDEFAULT);
	return 0;
}

LRESULT CMainDlg::OnPreferences(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CPreferencesDlg dlg;
	dlg.SetPreferences(m_pref);
	if (dlg.DoModal() == IDOK)
	{
		m_pref = dlg.GetPreferences();
	}
	return 0;
}

LRESULT CMainDlg::OnListClick(LPNMHDR pnmh)
{
	auto pnmia = reinterpret_cast<LPNMITEMACTIVATE>(pnmh);

	CString sItem;
	m_lvMessages.GetItemText(pnmia->iItem, 0, sItem);
	m_edMessage.SetWindowText(sItem);
	return 0;
}

LRESULT CMainDlg::OnListDblClick(LPNMHDR pnmh)
{
	auto pnmia = reinterpret_cast<LPNMITEMACTIVATE>(pnmh);

	CString sItem;
	m_lvMessages.GetItemText(pnmia->iItem, 0, sItem);
	CString pos;
	int p = sItem.Find(_T("in line "));
	if (p != -1)
	{
		pos = sItem.Mid(p + 8);
		int l = _tstoi(pos.GetBuffer(20));
		CString sMMLFile;
		TCHAR szTemp[1024];
		m_edMMLFile.GetWindowText(szTemp, 1024); sMMLFile = szTemp;
		m_pref.EditMMLFile(*this, sMMLFile, l);

	}
	return 0;
}

LRESULT CMainDlg::OnDropFiles(HDROP hDrop)
{
	DWORD dwFileNameSize = ::DragQueryFile(hDrop, 0, nullptr, 0);
	TCHAR* pszDroppedFile = new TCHAR[dwFileNameSize + 1];
	::DragQueryFile(hDrop, 0, pszDroppedFile, dwFileNameSize + 1);
	m_edMMLFile.SetWindowText(pszDroppedFile);
	// smf ファイル名を自動生成
	TCHAR szText[1024];
	m_edMMLFile.GetWindowText(szText, 1024);
	CString src(szText);
	src = src.Left(src.ReverseFind('.')) + _T(".mid");
	m_edSMFFile.SetWindowText(src.GetBuffer(40));

	return 0;
}
