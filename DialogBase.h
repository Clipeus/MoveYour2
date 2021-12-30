#pragma once

class DialogBase
{
public:
  DialogBase(LPCTSTR lpTemplateName = nullptr);
  ~DialogBase();

public:
  virtual INT_PTR DoModal(HWND hWnd, LPCTSTR lpTemplateName = nullptr);

protected:
  virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static INT_PTR WINAPI s_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnDestroy(HWND hWnd);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);

protected:
  LPCTSTR m_lpTemplateName = nullptr;
  HWND m_hWnd = nullptr;
};

