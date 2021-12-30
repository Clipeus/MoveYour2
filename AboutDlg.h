#pragma once

#include "DialogBase.h"

class AboutDlg : public DialogBase
{
public:
  AboutDlg();

private:
  BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
};
