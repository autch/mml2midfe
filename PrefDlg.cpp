
#include "stdafx.h"
#include "resource.h"

#include "prefdlg.h"
#include "Preferences.h"
#include ".\prefdlg.h"

namespace
{
  CString MakeStringFromBuffer(CEdit& e)
  {
    TCHAR szBuffer[1024];
    e.GetWindowText(szBuffer, sizeof szBuffer);
    return CString(szBuffer);
  }
}

LRESULT CPreferencesDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());

  m_edMML2MID = GetDlgItem(IDC_MML2MIDEDIT);
  m_edMyEditorWithLine = GetDlgItem(IDC_MYEDITORWITHLINEEDIT);
  m_edMyPlayer = GetDlgItem(IDC_MYPLAYEREDIT);
  
  m_edMML2MID.SetWindowText(m_pref.GetMML2MIDSetting().GetBuffer(20));
  m_edMyEditorWithLine.SetWindowText(m_pref.GetMyEditorWithLineSetting().GetBuffer(20));
  m_edMyPlayer.SetWindowText(m_pref.GetMyPlayerSetting().GetBuffer(20));

  CButton btnRadioOn, btnRadioOff;
  if(m_pref.GetUseMyEditor())
  {
    btnRadioOn = GetDlgItem(IDC_USEMYEDITORRADIO);
    btnRadioOff = GetDlgItem(IDC_USEASSOCEDITORRADIO);
  }
  else
  {
    btnRadioOn = GetDlgItem(IDC_USEASSOCEDITORRADIO);
    btnRadioOff = GetDlgItem(IDC_USEMYEDITORRADIO);
  }
  btnRadioOn.SetCheck(TRUE); btnRadioOff.SetCheck(FALSE);

  if(m_pref.GetUseMyPlayer())
  {
    btnRadioOn = GetDlgItem(IDC_USEMYPLAYERRADIO);
    btnRadioOff = GetDlgItem(IDC_USEASSOCPLAYERRADIO);
  }
  else
  {
    btnRadioOn = GetDlgItem(IDC_USEASSOCPLAYERRADIO);
    btnRadioOff = GetDlgItem(IDC_USEMYPLAYERRADIO);
  }
  btnRadioOn.SetCheck(TRUE); btnRadioOff.SetCheck(FALSE);

  return TRUE;
}

LRESULT CPreferencesDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if(wID == IDOK)
  {
    m_pref.SetMML2MIDSetting(MakeStringFromBuffer(m_edMML2MID));
    m_pref.SetMyEditorWithLineSetting(MakeStringFromBuffer(m_edMyEditorWithLine));
    m_pref.SetMyPlayerSetting(MakeStringFromBuffer(m_edMyPlayer));

    m_pref.SetUseMyEditor(GetDialogItem<CButton>(IDC_USEMYEDITORRADIO).GetCheck() != 0);
    m_pref.SetUseMyPlayer(GetDialogItem<CButton>(IDC_USEMYPLAYERRADIO).GetCheck() != 0);

  }
	EndDialog(wID);
	return 0;
}

LRESULT CPreferencesDlg::OnBnClickedBrowsemml2midbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  CFileDialog fdOpenFile(TRUE, _T("mml2mid.exe"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, _T("mml2mid.exe\0mml2mid.exe\0すべてのファイル\0*.*\0\0"));

  //  m_edMML2MID.GetWindowText(fdOpenFile.m_szFileName, sizeof fdOpenFile.m_szFileName);
  if(fdOpenFile.DoModal() == IDOK)
    m_edMML2MID.SetWindowText(fdOpenFile.m_szFileName);

  return 0;
}

LRESULT CPreferencesDlg::OnBnClickedBrowsemyeditorwithlinebutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  CFileDialog fdOpenFile(TRUE, _T("*.exe;*.cmd;*.bat;*.com"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, _T("プログラム\0*.exe;*.cmd;*.bat;*.com\0すべてのファイル\0*.*\0\0"));

  //m_edMyEditorWithLine.GetWindowText(fdOpenFile.m_szFileName, sizeof fdOpenFile.m_szFileName);
  if(fdOpenFile.DoModal() == IDOK)
    m_edMyEditorWithLine.SetWindowText(fdOpenFile.m_szFileName);
  return 0;
}

LRESULT CPreferencesDlg::OnBnClickedBrowsemyplayerbutton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  CFileDialog fdOpenFile(TRUE, _T("*.exe;*.cmd;*.bat;*.com"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, _T("プログラム\0*.exe;*.cmd;*.bat;*.com\0すべてのファイル\0*.*\0\0"));

  //m_edMyPlayer.GetWindowText(fdOpenFile.m_szFileName, sizeof fdOpenFile.m_szFileName);
  if(fdOpenFile.DoModal() == IDOK)
    m_edMyPlayer.SetWindowText(fdOpenFile.m_szFileName);

  return 0;
}
