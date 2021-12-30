#pragma once

#include <vector>

class FileData
{
public:
  FileData();
  ~FileData();

public:
  BOOL UpdateRecord(HWND hListhWnd);
  BOOL FillList(HWND hListhWnd);
  BOOL OnSelectFile(HWND hWnd, HINSTANCE hInstance, BOOL bOpen, LPTSTR lpszName);
  BOOL OnExportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance);
  BOOL OnInportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance, LPCTSTR lpszFileName = NULL);
  BOOL OnPaste(HWND hWnd, HWND hListView);
  BOOL OnCopyCut(HWND hWnd, HWND hListView, BOOL bCut);

private:
  BOOL DoGetFileRecord(HWND hListhWnd);
  DWORD getLen();
  void TrimPath(LPTSTR lpszPath);
  LPTSTR CheckHeader(LPTSTR lpBuf);
  BOOL DoUpdateRecord(HWND hListhWnd);
  BOOL SetEQU(HWND hListhWnd);

public:
  static LPCTSTR s_szSysKey;

private:
  std::vector<uint8_t> m_vecRegBuf;
};
