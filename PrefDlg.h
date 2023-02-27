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
		return { GetDlgItem(wID) };
	}
public:
	enum { IDD = IDD_PREFERENCES };

	BEGIN_MSG_MAP(CPreferencesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER_EX(IDC_BROWSEMML2MIDBUTTON, BN_CLICKED, OnBnClickedBrowseMML2MIDButton)
		COMMAND_HANDLER_EX(IDC_BROWSEMYEDITORWITHLINEBUTTON, BN_CLICKED, OnBnClickedBrowseMyEditorWithLineButton)
		COMMAND_HANDLER_EX(IDC_BROWSEMYPLAYERBUTTON, BN_CLICKED, OnBnClickedBrowseMyPlayerButton)
	END_MSG_MAP()

	void SetPreferences(CPreferences pref) { m_pref = pref; }
	CPreferences GetPreferences() { return m_pref; }

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBrowseMML2MIDButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnBnClickedBrowseMyEditorButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnBnClickedBrowseMyEditorWithLineButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnBnClickedBrowseMyPlayerButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
};
