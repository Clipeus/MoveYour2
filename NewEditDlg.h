#pragma once

#include "DialogBase.h"

class NewEditDlg : public DialogBase
{
public:
  NewEditDlg();

public:
  void SetItemData(LPITEMDATAEX pItemDataEx)
  {
    m_pItemDataEx = pItemDataEx;
  }
  LPITEMDATAEX GetItemData() const
  {
    return m_pItemDataEx;
  }

private:
  void FillDlgItems(HWND hWnd, LPITEMDATAEX lpItemDataEx);
  BOOL FillListItems(HWND hWnd, LPITEMDATAEX lpItemDataEx);

private:
  BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);

private:
  LPITEMDATAEX m_pItemDataEx;
};

