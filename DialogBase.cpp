#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "DialogBase.h"
#include "Utils.h"

DialogBase::DialogBase(LPCTSTR lpTemplateName /*= nullptr*/) : m_lpTemplateName(lpTemplateName)
{

}

DialogBase::~DialogBase()
{

}

INT_PTR DialogBase::DoModal(HWND hWnd, LPCTSTR lpTemplateName /*= nullptr*/)
{
  return ::DialogBoxParam(MoveYourApp::GetApp()->GetInstance(), 
    lpTemplateName != nullptr ? lpTemplateName : m_lpTemplateName, hWnd, s_DlgProc, reinterpret_cast<LPARAM>(this));
}

void DialogBase::OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
  switch (id)
  {
    case IDOK:
    case IDCANCEL:
    {
      EndDialog(hWnd, id);
      return;
    }
  }
}

void DialogBase::OnDestroy(HWND hWnd)
{
  DestroyIcon((HICON)SendMessage(hWnd, WM_GETICON, TRUE, 0));
  DestroyIcon((HICON)SendMessage(hWnd, WM_GETICON, FALSE, 0));
}

BOOL DialogBase::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
  SendMessage(hWnd, WM_SETICON, TRUE, (LPARAM)LoadIcon(MoveYourApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MAIN_ICON)));
  SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)LoadIcon(MoveYourApp::GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_MAIN_ICON)));
  return TRUE;
}

BOOL DialogBase::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    HANDLE_DLGMSG(hWnd, WM_DESTROY, OnDestroy);
    HANDLE_DLGMSG(hWnd, WM_INITDIALOG, OnInitDialog);
    HANDLE_DLGMSG(hWnd, WM_COMMAND, OnCommand);
  }
  return FALSE;
}

INT_PTR WINAPI DialogBase::s_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  DialogBase* dlg = reinterpret_cast<DialogBase*>(GetWindowLongPtr(hWnd, DWLP_USER));

  if (uMsg == WM_INITDIALOG)
  {
    dlg = reinterpret_cast<DialogBase*>(lParam);
    _ASSERTE(dlg);
    if (dlg)
      dlg->m_hWnd = hWnd;
    SetWindowLongPtr(hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(dlg));
  }

  if (dlg)
    return dlg->DlgProc(hWnd, uMsg, wParam, lParam);

  return FALSE;
}

