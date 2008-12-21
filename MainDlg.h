// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Preferences.h"

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
private:
	void CloseDialog(int nVal);
  BOOL AddMessage(CString s);
  int DecideIcon(CString& s);
  CString MakeupCommandLine();

  template<class T>
  T GetDialogItem(WORD wID)
  {
    T t = GetDlgItem(wID);
    return t;
  }

  CPreferences m_pref;
  CEdit m_edMMLFile, m_edSMFFile, m_edTranspose, m_edMessage;
  CListViewCtrl m_lvMessages;
  CUpDownCtrl m_udTranspose;
  CAccelerator m_acc;
  CMenu m_mnuMenu;
  //TCHAR m_szAppPath[MAX_PATH];
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
    UPDATE_ELEMENT(IDC_MMLFILEEDIT, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_MESSAGELIST, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_COMPILEBUTTON, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_PLAYBUTTON, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_EDITBUTTON, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_SMFFORMAT0RADIO, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_SMFFORMAT1RADIO, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_INVERTOCTCHECK, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_INVERTVOLCHECK, UPDUI_CHILDWINDOW)
    UPDATE_ELEMENT(IDC_TRANSPOSEEDIT, UPDUI_CHILDWINDOW)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_DROPFILES(OnDropFiles)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
    COMMAND_HANDLER_EX(IDC_BROWSEMMLBUTTON, BN_CLICKED, OnBnClickedBrowsemmlbutton)
    COMMAND_HANDLER_EX(IDC_BROWSESMFBUTTON, BN_CLICKED, OnBnClickedBrowsesmfbutton)
    COMMAND_HANDLER_EX(IDC_SMFFILEEDIT, EN_CHANGE, OnEnFileedit)
    COMMAND_HANDLER_EX(IDC_MMLFILEEDIT, EN_CHANGE, OnEnFileedit)
    COMMAND_HANDLER_EX(IDC_MMLFILEEDIT, EN_SETFOCUS, OnEnSetfocusFileedit)
    COMMAND_HANDLER_EX(IDC_SMFFILEEDIT, EN_SETFOCUS, OnEnSetfocusFileedit)
    COMMAND_HANDLER_EX(IDC_COMPILEBUTTON, BN_CLICKED, OnBnClickedCompilebutton)
    COMMAND_HANDLER_EX(IDC_TRANSPOSEEDIT, EN_CHANGE, OnEnTransposeedit)
    COMMAND_HANDLER_EX(IDC_PLAYBUTTON, BN_CLICKED, OnBnClickedPlaybutton)
    COMMAND_HANDLER_EX(IDC_EDITBUTTON, BN_CLICKED, OnBnClickedEditbutton)

    COMMAND_ID_HANDLER_EX(IDM_ABOUT, OnAbout)
    COMMAND_ID_HANDLER_EX(IDM_OPENMML, OnBnClickedBrowsemmlbutton)
    COMMAND_ID_HANDLER_EX(IDM_COMPILE, OnBnClickedCompilebutton)
    COMMAND_ID_HANDLER_EX(IDM_PLAY, OnBnClickedPlaybutton)
    COMMAND_ID_HANDLER_EX(IDM_EDIT, OnBnClickedEditbutton)
    COMMAND_ID_HANDLER_EX(IDM_EXIT, OnCancel)

    COMMAND_ID_HANDLER_EX(IDM_MML2MID_WEB, OnMML2MIDWeb)
    COMMAND_ID_HANDLER_EX(IDM_AUTCH_NET, OnAutchNet)
    COMMAND_ID_HANDLER_EX(IDM_HELP_CONTENTS, OnHelpContents)
    COMMAND_ID_HANDLER_EX(IDM_PREFERENCES, OnPreferences)
    NOTIFY_HANDLER_EX(IDC_MESSAGELIST, NM_DBLCLK, OnListDblClick)
    NOTIFY_HANDLER_EX(IDC_MESSAGELIST, NM_CLICK, OnListClick)
    CHAIN_MSG_MAP(CUpdateUI<CMainDlg>)
  END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/)

	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

  LRESULT OnDropFiles(HDROP /*hDrop*/);

  LRESULT OnBnClickedBrowsemmlbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnBnClickedBrowsesmfbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnEnSetfocusFileedit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnBnClickedCompilebutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnEnTransposeedit(UINT uNotifyCode, int nID, HWND hWndCtl);
  LRESULT OnEnFileedit(UINT uNotifyCode, int nID, HWND hWndCtl);
  LRESULT OnAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnBnClickedPlaybutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnBnClickedEditbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnMML2MIDWeb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnAutchNet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnHelpContents(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnPreferences(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnListDblClick(LPNMHDR pnmh);
  LRESULT OnListClick(LPNMHDR pnmh);
};
