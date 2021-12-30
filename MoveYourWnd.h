#pragma once

#include <memory>
#include "WindowBase.h"

class FileData;
class FindDlg;
class RegMonitor;

class MoveYourWnd : public WindowBase
{
public:
  MoveYourWnd();
  ~MoveYourWnd();

public:
  static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  bool Init();
  bool Create(int nCmdShow);

private:
  void EnableCommands(long nCount, long nSelCount, HMENU hContextMenu = NULL);
  void SetStdStatusText(long nCount = -1, long nSelCount = -1);
  void SetStdStatusAndMenu();
  void ChangeMenuItems(HMENU hContextMenu = NULL);
  void ReadWindowPos(LPCTSTR lpszFolder, LPCTSTR lpszKey);
  void WriteWindowPos(LPCTSTR lpszFolder, LPCTSTR lpszKey) const;
  void AdjustRect(int x, int y, int cx, int cy, BOOL bMove = TRUE);
  void DrawImage();
  void InitList();
  void UninitList();
  void InsertItem(LPITEMDATA lpFileData, long nIndex);
  void AddToList(LPITEMDATA lpFileData);
  BOOL SetListItemData(LPITEMDATA lpFileData, long nIndex = -1);
  BOOL GetListItemData(LPITEMDATA lpFileData, long nIndex = -1);
  BOOL GetListItemData(LPITEMDATA lpFileData, BOOL bSelection, int& nIndex);
  void DrawColumnImage(HWND hHeader, int nColumn, HBITMAP hBmp);
  void SortItems(short nColumn, HBITMAP hUp, HBITMAP hDown);
  int GetStartItem();
  BOOL OnEndFind(int nIndex);
  BOOL OnFind(BOOL bNext = FALSE);

  static LRESULT CALLBACK HeaderProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static int CALLBACK CompareListItem(LPITEMDATA lpFileData1, LPITEMDATA lpFileData2, long nSortData);

private:
  LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnDestroy(HWND hWnd);
  BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
  void OnSize(HWND hWnd, UINT state, int cx, int cy);
  void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
  void OnMenuSelect(HWND hWnd, HMENU hMenu, int item, HMENU hMenuPopup, UINT flags);
  void OnContextMenu(HWND hWnd, HWND hwndContext, UINT x, UINT y);
  LRESULT OnNotify(HWND hWnd, int idFrom, NMHDR* pNmndr);
  BOOL OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
  void OnActivate(HWND hWnd, UINT nState, HWND hWndActDeact, BOOL fMinimized);
  void OnMeasureItem(HWND hWnd, MEASUREITEMSTRUCT* lpMeasureItem);
  void OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT* lpDrawItem);
  void OnInitMenuPopup(HWND hWnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
  void OnNCDestroy(HWND hWnd);
  void OnChangeCBChain(HWND hWnd, HWND hWndRemove, HWND hWndNext);
  void OnDrawClipboard(HWND hWnd);
  void OnDropFiles(HWND hWnd, HDROP hDrop);

public:
  static LPCTSTR s_szWindowClass;

private:
  HWND m_hToolBar = nullptr;
  HWND m_hListView = nullptr;
  HWND m_hStatusBar = nullptr;
  HWND m_hClipNextView = nullptr;
  HBITMAP m_hUp = nullptr;
  HBITMAP m_hDown = nullptr;
  HBITMAP m_hBitmap = nullptr;
  WNDPROC m_lfOldWndPros = nullptr;
  int m_ptWidth[2] = { 0, 0 };
  std::unique_ptr<FileData> m_pFileData;
  std::unique_ptr<FindDlg> m_pFindDlg;
  std::unique_ptr<RegMonitor> m_pRegMonitor;

  friend class FileData;

};

