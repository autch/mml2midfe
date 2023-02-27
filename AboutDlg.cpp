// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "CVersionInfo.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	CStatic stVersion{ GetDlgItem(IDC_VERSIONLABEL) };

	const CVersionInfo info(nullptr);
	CHAR szStringFileVersion[32];
	info.FormatFileVersion(szStringFileVersion, sizeof szStringFileVersion);
	CHAR szFormattedVersion[128];
	sprintf_s(szFormattedVersion, sizeof szFormattedVersion, "Version %s", szStringFileVersion);

	stVersion.SetWindowText(CString(szFormattedVersion));

	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
