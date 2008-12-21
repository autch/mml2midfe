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
  if((!m_acc.IsNull() && m_acc.TranslateAccelerator(m_hWnd, pMsg)))
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
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
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

  // �g�����X�|�[�Y�֌W
  m_udTranspose = GetDlgItem(IDC_TRSANSPOSESPIN); m_udTranspose.SetRange(-12, 12);
  m_edTranspose = GetDlgItem(IDC_TRANSPOSEEDIT); m_edTranspose.SetWindowText("0");

  // �f�t�H���g�� Format 1 ���g��
  UISetCheck(IDC_SMFFORMAT1RADIO, TRUE);

  // ���X�g�r���[�̏���
  m_lvMessages = GetDlgItem(IDC_MESSAGELIST);
  m_lvMessages.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

  {
    CImageList il;
    il.Create(16, 16, ILC_COLOR8 | ILC_MASK, 2, 1);

    // �r�b�g�}�b�v���\�[�X���C���[�W���X�g�ɒǉ�
    { CBitmap bmp; bmp.LoadBitmap(IDB_INFO); il.Add(bmp, RGB(255, 0, 255)); }
    { CBitmap bmp; bmp.LoadBitmap(IDB_WARNING); il.Add(bmp, RGB(255, 0, 255)); }
    { CBitmap bmp; bmp.LoadBitmap(IDB_ERROR); il.Add(bmp, RGB(255, 0, 255)); }

    // �C���[�W���X�g�����X�g�r���[�R���g���[���ɐݒ�
    m_lvMessages.SetImageList(il, LVSIL_SMALL);
  }

  // ���X�g�r���[�ɃJ������p�ӂ���
  {
    CRect rcList;
    m_lvMessages.GetWindowRect(rcList);
    int nScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
    int n3DEdge = GetSystemMetrics(SM_CXEDGE);
    m_lvMessages.InsertColumn(0, _T("���b�Z�[�W"), LVCFMT_LEFT,
                              rcList.Width() - nScrollWidth - n3DEdge * 2, -1);
  }
  UIEnable(IDC_COMPILEBUTTON, FALSE);
  UIEnable(IDC_EDITBUTTON, FALSE);
  UIEnable(IDC_PLAYBUTTON, FALSE);

  m_mnuMenu = GetMenu();
  m_mnuMenu.EnableMenuItem(IDM_COMPILE, MF_GRAYED);
  m_mnuMenu.EnableMenuItem(IDM_PLAY, MF_GRAYED);
  m_mnuMenu.EnableMenuItem(IDM_EDIT, MF_GRAYED);

  CVersionInfo info(NULL);
  CHAR szStringFileVersion[32];
  info.FormatFileVersion(szStringFileVersion, sizeof szStringFileVersion);
  CHAR szFormattedVersion[128];
  sprintf_s(szFormattedVersion, sizeof szFormattedVersion, "MML2MIDfe Version %s", szStringFileVersion);

  AddMessage(_T(szFormattedVersion));

  // mml2mid �̃o�[�W���������擾
  CString s;
  if(GetMML2MIDBanner(m_pref.GetMML2MIDSetting(), s))
    AddMessage(s);
  else
    AddMessage(_T("ERROR! mml2mid.exe ��������܂���ł����D\r\n�u�ݒ�v�Ő������p�X���w�肵�Ă��������D"));

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
  // TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
  CFileDialog fdOpenFile(TRUE, _T("*.mml"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, _T("MML �t�@�C�� (*.mml)\0*.mml\0���ׂẴt�@�C��\0*.*\0\0"));

  m_edMMLFile.GetWindowText(fdOpenFile.m_szFileName, sizeof fdOpenFile.m_szFileName);
  if(fdOpenFile.DoModal() == IDOK)
  {
    m_edMMLFile.SetWindowText(fdOpenFile.m_szFileName);

    // smf �t�@�C��������������
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
  // TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
  CFileDialog fdOpenFile(FALSE, _T("*.mid"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, _T("�W�� MIDI �t�@�C�� (*.mid)\0*.mid\0���ׂẴt�@�C��\0*.*\0\0"));

  m_edSMFFile.GetWindowText(fdOpenFile.m_szFileName, sizeof fdOpenFile.m_szFileName);
  if(fdOpenFile.DoModal() == IDOK)
  {
    m_edSMFFile.SetWindowText(fdOpenFile.m_szFileName);
  }

  return 0;
}

LRESULT CMainDlg::OnEnSetfocusFileedit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
  // TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
  GetDialogItem<CEdit>(wID).SetSelAll();

  return 0;
}

LRESULT CMainDlg::OnBnClickedCompilebutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  // TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
  CString sStdOut, sStdErr;
  CString sCmdLine = MakeupCommandLine();

  m_lvMessages.DeleteAllItems();
  AddMessage(sCmdLine);
 
  RunMML2MID(sCmdLine, sStdOut, sStdErr);

  if(AddMessage(sStdOut))
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
  while(s.GetLength())
  {
    CString part;
    int p = s.Find("\x0d\x0a");
    if(p == -1)
    {
      part = s;
      s = "";
    }
    else
    {
      part = s.Left(p);
      s = s.Mid(p + 2);
    }
    int nIndex = DecideIcon(part);
    if(nIndex == 2) f = FALSE;
    m_lvMessages.AddItem(m_lvMessages.GetItemCount(), 0, part.GetBuffer(64), nIndex);
  }
  m_lvMessages.EnsureVisible(m_lvMessages.GetItemCount() - 1, FALSE);
  return f;
}

int CMainDlg::DecideIcon(CString& s)
{
  // ���b�Z�[�W���e����A�C�R�������߂�
  if(s.Find("ERROR! ") != -1)
    return 2;
  if(s.Find("Warning: ") != -1)
    return 1;

  return 0;
}

LRESULT CMainDlg::OnEnTransposeedit(UINT uNotifyCode, int nID, HWND hWndCtl)
{
  if(m_udTranspose.m_hWnd != NULL)
  {
    BOOL bError;
    m_udTranspose.GetPos(&bError);
    if(bError)
      m_udTranspose.SetPos(0);
  }
  return 0;
}

LRESULT CMainDlg::OnAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  // TODO : �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
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

  sCmdLine.Format("\"%s\" ", m_pref.GetMML2MIDSetting());
  if(GetDialogItem<CButton>(IDC_INVERTOCTCHECK).GetCheck())
    sCmdLine += "-x ";
  if(GetDialogItem<CButton>(IDC_INVERTVOLCHECK).GetCheck())
    sCmdLine += "-v ";
  if(GetDialogItem<CButton>(IDC_SMFFORMAT0RADIO).GetCheck())
    sCmdLine += "-f ";
  GetDialogItem<CEdit>(IDC_TRANSPOSEEDIT).GetWindowText(szTemp, sizeof szTemp);
  if(atoi(szTemp) != 0)
  {
    sTemp.Format("-t%s ", szTemp);
    sCmdLine += sTemp;
  }

  CString sMMLFile, sSMFFile;

  m_edMMLFile.GetWindowText(szTemp, 1024); sMMLFile = szTemp;
  m_edSMFFile.GetWindowText(szTemp, 1024); sSMFFile = szTemp;

  sTemp.Format("\"%s\" \"%s\"", sMMLFile, sSMFFile);
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
  ShellExecute(*this, NULL, _T("http://platz.jp/~mml2mid/"), NULL, NULL, SW_SHOWDEFAULT);
  return 0;
}
LRESULT CMainDlg::OnAutchNet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  ShellExecute(*this, NULL, _T("http://www.autch.net/"), NULL, NULL, SW_SHOWDEFAULT);
  return 0;
}
LRESULT CMainDlg::OnHelpContents(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  // TODO : �w���v�t�@�C�������
  ShellExecute(*this, NULL, _T("mml2midfe.txt"), NULL, NULL, SW_SHOWDEFAULT);
  return 0;
}

LRESULT CMainDlg::OnPreferences(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  CPreferencesDlg dlg;
  dlg.SetPreferences(m_pref);
  if(dlg.DoModal() == IDOK)
  {
    m_pref = dlg.GetPreferences();
  }
  return 0;
}

LRESULT CMainDlg::OnListClick(LPNMHDR pnmh)
{
  LPNMITEMACTIVATE pnmia = (LPNMITEMACTIVATE)pnmh;

  CString sItem;
  m_lvMessages.GetItemText(pnmia->iItem, 0, sItem);
  m_edMessage.SetWindowText(sItem);
  return 0;
}

LRESULT CMainDlg::OnListDblClick(LPNMHDR pnmh)
{
  LPNMITEMACTIVATE pnmia = (LPNMITEMACTIVATE)pnmh;

  CString sItem;
  m_lvMessages.GetItemText(pnmia->iItem, 0, sItem);
  CString pos;
  int p = sItem.Find(_T("in line "));
  if(p != -1)
  {
    pos = sItem.Mid(p + 8);
    int l = atoi(pos.GetBuffer(20));
    CString sMMLFile;
    TCHAR szTemp[1024];
    m_edMMLFile.GetWindowText(szTemp, 1024); sMMLFile = szTemp;
    m_pref.EditMMLFile(*this, sMMLFile, l);

  }
  return 0;
}

LRESULT CMainDlg::OnDropFiles(HDROP hDrop)
{
  PCHAR pszDroppedFile = NULL;
  DWORD dwFileNameSize = 0;

  dwFileNameSize = ::DragQueryFile(hDrop, 0, NULL, 0);
  pszDroppedFile = new CHAR[dwFileNameSize + 1];
  ::DragQueryFile(hDrop, 0, pszDroppedFile, dwFileNameSize + 1);
  m_edMMLFile.SetWindowText(pszDroppedFile);
  // smf �t�@�C��������������
  TCHAR szText[1024];
  m_edMMLFile.GetWindowText(szText, 1024);
  CString src(szText);
  src = src.Left(src.ReverseFind('.')) + _T(".mid");
  m_edSMFFile.SetWindowText(src.GetBuffer(40));

  return 0;
}
