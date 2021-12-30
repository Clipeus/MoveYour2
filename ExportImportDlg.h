#pragma once
#include "DialogBase.h"

class ExportImportDlg : public DialogBase
{
public:
  ExportImportDlg();

public:
  INT_PTR DoExportModal(HWND hWnd);
  INT_PTR DoImportModal(HWND hWnd);

private:
  BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
};

