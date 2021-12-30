#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <stdio.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "AppOptions.h"
#include "OptionsDlg.h"
#include "Utils.h"

OptionsDlg::OptionsDlg()
{

}

INT_PTR OptionsDlg::DoModal(HWND hWnd, LPCTSTR lpTemplateName /*= nullptr*/)
{
  TCHAR szCaption[256];
  PROPSHEETPAGE PSheetPage[1];
  PROPSHEETHEADER PSheetHeader;
  LoadString(MoveYourApp::GetApp()->GetInstance(), IDS_CAPTION_SET, szCaption, 256);

  PSheetPage[0].dwSize = sizeof(PROPSHEETPAGE);
  PSheetPage[0].dwFlags = PSP_DEFAULT;
  PSheetPage[0].hInstance = MoveYourApp::GetApp()->GetInstance();
  PSheetPage[0].pszIcon = 0;
  PSheetPage[0].pszTemplate = MAKEINTRESOURCE(IDD_GENERAL);
  PSheetPage[0].pfnDlgProc = s_DlgProc;
  PSheetPage[0].pszTitle = NULL;
  PSheetPage[0].lParam = reinterpret_cast<LPARAM>(this);

  PSheetHeader.dwSize = sizeof(PROPSHEETHEADER);
  PSheetHeader.dwFlags = PSH_USEICONID | PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
  PSheetHeader.hInstance = MoveYourApp::GetApp()->GetInstance();
  PSheetHeader.pszIcon = MAKEINTRESOURCE(IDI_MAIN_ICON);
  PSheetHeader.hwndParent = hWnd;
  PSheetHeader.pszCaption = szCaption;
  PSheetHeader.nPages = 1;
  PSheetHeader.nStartPage = 0;
  PSheetHeader.ppsp = (LPPROPSHEETPAGE)&PSheetPage;

  return PropertySheet(&PSheetHeader);
}

void OptionsDlg::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  EnableWindow(GetDlgItem(hWnd, IDC_RADIO1), (SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
  EnableWindow(GetDlgItem(hWnd, IDC_RADIO2), (SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
  EnableWindow(GetDlgItem(hWnd, IDC_RADIO3), (SendDlgItemMessage(hWnd, IDC_CHECK3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
  EnableWindow(GetDlgItem(hWnd, IDC_RADIO4), (SendDlgItemMessage(hWnd, IDC_CHECK3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED));
}

LRESULT OptionsDlg::OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr)
{
  if (pNmndr->code == PSN_APPLY)
  {
    MoveYourApp::GetApp()->GetOptions()->bURAS = (SendDlgItemMessage(hWnd, IDC_CHECK1, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    MoveYourApp::GetApp()->GetOptions()->bDNSDWIF = (SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    MoveYourApp::GetApp()->GetOptions()->bDNSDWEF = (SendDlgItemMessage(hWnd, IDC_CHECK3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    MoveYourApp::GetApp()->GetOptions()->bMI = (SendDlgItemMessage(hWnd, IDC_RADIO1, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
    MoveYourApp::GetApp()->GetOptions()->bAI = (SendDlgItemMessage(hWnd, IDC_RADIO3, BM_GETCHECK, BST_CHECKED, 0) == BST_CHECKED);
  }
  return FALSE;
}

BOOL OptionsDlg::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  if (MoveYourApp::GetApp()->GetOptions()->bURAS)
    SendDlgItemMessage(hWnd, IDC_CHECK1, BM_SETCHECK, BST_CHECKED, 0);

  if (MoveYourApp::GetApp()->GetOptions()->bDNSDWIF)
  {
    SendDlgItemMessage(hWnd, IDC_CHECK2, BM_SETCHECK, BST_CHECKED, 0);
    if (MoveYourApp::GetApp()->GetOptions()->bMI)
      SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
    else
      SendDlgItemMessage(hWnd, IDC_RADIO2, BM_SETCHECK, BST_CHECKED, 0);
  }
  else
  {
    SendDlgItemMessage(hWnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
    EnableWindow(GetDlgItem(hWnd, IDC_RADIO1), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_RADIO2), FALSE);
  }
  if (MoveYourApp::GetApp()->GetOptions()->bDNSDWEF)
  {
    SendDlgItemMessage(hWnd, IDC_CHECK3, BM_SETCHECK, BST_CHECKED, 0);
    if (MoveYourApp::GetApp()->GetOptions()->bAI)
      SendDlgItemMessage(hWnd, IDC_RADIO3, BM_SETCHECK, BST_CHECKED, 0);
    else
      SendDlgItemMessage(hWnd, IDC_RADIO4, BM_SETCHECK, BST_CHECKED, 0);
  }
  else
  {
    SendDlgItemMessage(hWnd, IDC_RADIO3, BM_SETCHECK, BST_CHECKED, 0);
    EnableWindow(GetDlgItem(hWnd, IDC_RADIO3), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_RADIO4), FALSE);
  }

  return DialogBase::OnInitDialog(hWnd, hWndFocus, lParam);
}

BOOL OptionsDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_DESTROY, OnDestroy);
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
    HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
  }
  return DialogBase::DlgProc(hWnd, uMsg, wParam, lParam);
}

INT_PTR OptionsDlg::s_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  OptionsDlg* dlg = reinterpret_cast<OptionsDlg*>(GetWindowLongPtr(hWnd, DWLP_USER));

  if (uMsg == WM_INITDIALOG)
  {
    dlg = reinterpret_cast<OptionsDlg*>(reinterpret_cast<LPPROPSHEETPAGE>(lParam)->lParam);
    _ASSERTE(dlg);
    if (dlg)
      dlg->m_hWnd = hWnd;
    SetWindowLongPtr(hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(dlg));
  }

  if (dlg)
    return dlg->DlgProc(hWnd, uMsg, wParam, lParam);

  return SetDlgMsgResult(hWnd, uMsg, FALSE);
}

