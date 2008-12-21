
#pragma once

#include "Preferences.h"

class CPreferencesDlg : public CDialogImpl<CPreferencesDlg>
{
private:
  CPreferences m_pref;
  CEdit m_edMML2MID;
  CEdit m_edMyEditorWithLine;
  CEdit m_edMyPlayer;

  template<class T>
  T GetDialogItem(WORD wID)
  {
    T t = GetDlgItem(wID);
    return t;
  }
public:
	enum { IDD = IDD_PREFERENCES };

	BEGIN_MSG_MAP(CPreferencesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    COMMAND_HANDLER_EX(IDC_BROWSEMML2MIDBUTTON, BN_CLICKED, OnBnClickedBrowsemml2midbutton)
    COMMAND_HANDLER_EX(IDC_BROWSEMYEDITORWITHLINEBUTTON, BN_CLICKED, OnBnClickedBrowsemyeditorwithlinebutton)
    COMMAND_HANDLER_EX(IDC_BROWSEMYPLAYERBUTTON, BN_CLICKED, OnBnClickedBrowsemyplayerbutton)
  END_MSG_MAP()

  void SetPreferences(CPreferences pref) { m_pref = pref; }
  CPreferences GetPreferences() { return m_pref; }

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBrowsemml2midbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnBnClickedBrowsemyeditorbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnBnClickedBrowsemyeditorwithlinebutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
  LRESULT OnBnClickedBrowsemyplayerbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
};
