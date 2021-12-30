/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	List View
*/

#define STRICT
#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <stdio.h>
#include <tchar.h>
#include <memory>
#include "MoveYour.h"
#include "AppOptions.h"
#include "MoveYourApp.h"
#include "MoveYourWnd.h"
#include "FileData.h"
#include "Utils.h"
#include "ExportImportDlg.h"
#include "RegMonitor.h"

#define STEP	(sizeof(_T("\\??\\")) - sizeof(TCHAR)) / sizeof(TCHAR)
#define STEP2	(sizeof(_T("!\\??\\")) - sizeof(TCHAR)) / sizeof(TCHAR)

LPCTSTR FileData::s_szSysKey = _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager");
static TCHAR szSysVal[] = _T("PendingFileRenameOperations");

static TCHAR szSignature[] = _T("MOVEYOURFILES");
static TCHAR szPointer[] = _T(" > ");
static TCHAR szPointer2[] = _T("=");
static TCHAR szEOL[] = _T("\r\n");
static TCHAR szEO2L[] = _T("\r\n\r\n");
static TCHAR szNULL[] = _T("NUL");

FileData::FileData()
{

}

FileData::~FileData()
{

}

DWORD FileData::getLen()
{
  DWORD dwSize = 0;
  HKEY hKey;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, s_szSysKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
  {
    DWORD dwRealType;
    if ((RegQueryValueEx(hKey, szSysVal, 0, &dwRealType, NULL, &dwSize) != ERROR_SUCCESS)
      && (REG_MULTI_SZ == dwRealType))
    {
      dwSize = 0;
    }
    RegCloseKey(hKey);
  }
  return dwSize;
}

BOOL FileData::DoGetFileRecord(HWND hListhWnd)
{
  DWORD dwSize = getLen();
  if (!dwSize)
    return FALSE;

  std::unique_ptr<ITEMDATA> lpFileData;
  m_vecRegBuf.assign(dwSize + STEP, 0);

  if (!GetRegMString(s_szSysKey, szSysVal, NULL, m_vecRegBuf.data(), dwSize, HKEY_LOCAL_MACHINE))
  {
    ShowOSError();
    return false;
  }

  LPTSTR sz = (LPTSTR)m_vecRegBuf.data();
  for (int i = 0; *sz; i++)
  {
    BOOL bDelete = TRUE;
    lpFileData = std::make_unique<ITEMDATA>();
    _tcscpy_s(lpFileData->szSource, sz + STEP);
    _tcscpy_s(lpFileData->szDestination, sz +_tcslen(sz) + 1);
    long lStep = STEP;
    if (lpFileData->szDestination && lpFileData->szDestination[0])
    {
      if (lpFileData->szDestination[0] == _T('!'))
        lStep = STEP2;

      memmove(lpFileData->szDestination, lpFileData->szDestination + lStep, (_tcslen(lpFileData->szDestination) - lStep + 1) * sizeof(TCHAR));
      bDelete = FALSE;
    }
    sz = sz + STEP + _tcslen(lpFileData->szDestination) + 1 + (bDelete ? 0 : lStep) + _tcslen(lpFileData->szSource) + 1;
    MoveYourApp::GetApp()->GetMainWnd()->InsertItem(lpFileData.release(), i);
  }
  
  lpFileData.release();
  return TRUE;
}

void FileData::TrimPath(LPTSTR lpszPath)
{
  if (!lpszPath || !lpszPath[0])
    return;

  LPTSTR sz = lpszPath + _tcslen(lpszPath);
  sz = _tcsdec(lpszPath, sz);
  while (sz && _istspace(*sz))
  {
    sz = _tcsdec(lpszPath, sz);
  }
  if (sz)
  {
    *_tcsinc(sz) = 0;
  }

  sz = lpszPath;
  int i;
  for (i = 0; *sz && _istspace(*sz); i++)
  {
    sz = _tcsinc(sz);
  }
  if (i)
  {
    memmove(lpszPath, lpszPath + i, _tcslen(lpszPath) - i);
    lpszPath[_tcslen(lpszPath) - i] = 0;
  }
}

LPTSTR FileData::CheckHeader(LPTSTR lpBuf)
{
  LPTSTR sz = lpBuf;
  while (*sz && _istspace(*sz))
  {
    sz = _tcsinc(sz);
  }
  if (_tcsnicmp(sz, _T("[rename]\r\n"), sizeof(_T("[rename]\r\n")) - 1))
  {
    if (_tcsnicmp(sz, _T("[rename]"), sizeof(_T("[rename]")) - 1))
    {
      return NULL;
    }
    sz += sizeof(_T("[rename]")) - 1;
    while (*sz && _istspace(*sz) && _tccmp(sz, _T("\n")))
    {
      sz = _tcsinc(sz);
    }

    if ((*sz != NULL) && (!_tccmp(sz, _T("\n"))))
    {
      return sz;
    }
    else
    {
      return NULL;
    }
  }
  return sz;
}

BOOL FileData::DoUpdateRecord(HWND hListhWnd)
{
  return true;

  std::vector<uint8_t> vecRegBuf;
  std::unique_ptr<TCHAR[]> lpData;
  DWORD nLen = 0;
  DWORD dwSize = getLen();
  if (dwSize)
  {
    vecRegBuf.assign(dwSize + STEP, 0);
    if (!GetRegMString(s_szSysKey, szSysVal, NULL, vecRegBuf.data(), dwSize, HKEY_LOCAL_MACHINE))
    {
      ShowOSError();
      return false;
    }
  }

  if (!dwSize || m_vecRegBuf.data() != vecRegBuf.data())
  {
    lpData.reset(new TCHAR[STEP + (MAX_PATH * 2 * ListView_GetItemCount(hListhWnd))]);
    ITEMDATA FileData;
    for (int i = 0; MoveYourApp::GetApp()->GetMainWnd()->GetListItemData(&FileData, i); i++)
    {
      memcpy(lpData.get() + nLen, _T("\\??\\"), STEP);
      nLen += (STEP / sizeof(TCHAR));
      memcpy(lpData.get() + nLen, FileData.szSource, _tcslen(FileData.szSource));
      nLen += _tcslen(FileData.szSource);
      memcpy(lpData.get() + nLen, _T("\0"), sizeof(_T("\0")) - 1);
      nLen += sizeof(_T("\0")) - 1;
      if (FileData.szDestination[0])
      {
        memcpy(lpData.get() + nLen, _T("\\??\\"), STEP);
        nLen += (STEP / sizeof(TCHAR));
        memcpy(lpData.get() + nLen, FileData.szDestination, _tcslen(FileData.szDestination));
        nLen += _tcslen(FileData.szDestination);
      }
      memcpy(lpData.get() + nLen, _T("\0"), sizeof(_T("\0")) - 1);
      nLen += sizeof(_T("\0")) - 1;
    }
    if (nLen)
    {
      memcpy(lpData.get() + nLen, _T("\0"), sizeof(_T("\0")) - 1);
      nLen += sizeof(_T("\0")) - 1;
      if (!SetRegMString(s_szSysKey, szSysVal, lpData.get(), nLen, HKEY_LOCAL_MACHINE))
      {
        ShowOSError();
        return false;
      }
    }
    else
    {
      DeleteRegKey(s_szSysKey, szSysVal, HKEY_LOCAL_MACHINE);
    }
  }
  else
  {
    return SetEQU(hListhWnd);
  }

  if (!vecRegBuf.empty())
  {
    vecRegBuf.clear();
  }

  if (nLen && lpData)
  {
    m_vecRegBuf.reserve(nLen);
    memcpy(m_vecRegBuf.data(), lpData.get(), nLen);
  }

  return TRUE;
}

BOOL FileData::UpdateRecord(HWND hListhWnd)
{
  WaitCursor wc;
  MoveYourApp::GetApp()->GetMainWnd()->m_pRegMonitor->Suspend();
  BOOL bRet = DoUpdateRecord(hListhWnd);
  if (!bRet)
    SetEQU(hListhWnd);
  MoveYourApp::GetApp()->GetMainWnd()->m_pRegMonitor->Resume();
  return bRet;
}

BOOL FileData::FillList(HWND hListhWnd)
{
  WaitCursor wc;
  ListView_DeleteAllItems(hListhWnd);
  return DoGetFileRecord(hListhWnd);
}

BOOL FileData::OnSelectFile(HWND hWnd, HINSTANCE hInstance, BOOL bOpen, LPTSTR lpszName)
{
  if (!lpszName)
    return FALSE;

  OPENFILENAME ofn;
  TCHAR szFilter[256];

  LoadString(hInstance, IDS_SCRIPT_TEMP, szFilter, 255);
  LPTSTR lpszTemp = &szFilter[-1];
  while (true)
  {
    lpszTemp = _tcschr(&lpszTemp[1], _T('#'));
    if (lpszTemp)
      lpszTemp[0] = 0;
    else
      break;
  }

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWnd;
  ofn.hInstance = hInstance;
  ofn.lpstrFilter = szFilter;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = NULL;
  ofn.nFilterIndex = 1;;
  ofn.lpstrFile = lpszName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = NULL;
  ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | (bOpen ? OFN_FILEMUSTEXIST : 0) | OFN_HIDEREADONLY | OFN_EXPLORER;
  ofn.nFileOffset = NULL;
  ofn.nFileExtension = NULL;
  ofn.lpstrDefExt = _T("myf");
  ofn.lCustData = NULL;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;

  if ((bOpen ? GetOpenFileName(&ofn) : GetSaveFileName(&ofn)))
  {
    return TRUE;
  }
  return FALSE;
}

BOOL FileData::OnExportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance)
{
  TCHAR szFileName[MAX_PATH] = _T("Script.myf");
  if (!OnSelectFile(hWnd, hInstance, FALSE, szFileName))
    return FALSE;

  UINT nCode = MoveYourApp::GetApp()->GetOptions()->bAI ? 1 : 2;
  if (ListView_GetSelectedCount(hListhWnd) && !MoveYourApp::GetApp()->GetOptions()->bDNSDWEF)
  {
    ExportImportDlg dlg;
    nCode = dlg.DoExportModal(hWnd);
    if (!nCode || nCode == -1)
      return FALSE;
  }

  HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == hFile)
  {
    ShowOSError(IDS_FILE_CREATE_FAILED, GetLastError());
    return FALSE;
  }

  DWORD dwCheck = 0;
  if (!WriteFile(hFile, szSignature, _tcslen(szSignature), &dwCheck, NULL))
  {
    ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
    CloseHandle(hFile);
    return FALSE;
  }
  if (!WriteFile(hFile, szEO2L, sizeof(szEO2L), &dwCheck, NULL))
  {
    ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
    CloseHandle(hFile);
    return FALSE;
  }

  ITEMDATA FileData;
  int nIndex = -1;
  for (int i = 0; MoveYourApp::GetApp()->GetMainWnd()->GetListItemData(&FileData, nCode == 2, (nCode == 2) ? nIndex : i); i++)
  {
    if (!WriteFile(hFile, FileData.szSource, _tcslen(FileData.szSource), &dwCheck, NULL))
    {
      ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
      CloseHandle(hFile);
      return FALSE;
    }
    if (!WriteFile(hFile, szPointer, _tcslen(szPointer), &dwCheck, NULL))
    {
      ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
      CloseHandle(hFile);
      return FALSE;
    }
    if (FileData.szDestination[0])
    {
      if (!WriteFile(hFile, FileData.szDestination, _tcslen(FileData.szDestination), &dwCheck, NULL))
      {
        ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
        CloseHandle(hFile);
        return FALSE;
      }
    }
    else
    {
      if (!WriteFile(hFile, szNULL, _tcslen(szNULL), &dwCheck, NULL))
      {
        ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
        CloseHandle(hFile);
        return FALSE;
      }
    }
    if (!WriteFile(hFile, szEOL, sizeof(szEOL), &dwCheck, NULL))
    {
      ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
      CloseHandle(hFile);
      return FALSE;
    }
  }
  CloseHandle(hFile);
  return TRUE;
}

BOOL FileData::OnInportScript(HWND hWnd, HWND hListhWnd, HINSTANCE hInstance, LPCTSTR lpszFileName /*= NULL*/)
{
  TCHAR szFileName[MAX_PATH] = _T("");
  LPTSTR lpszBuf;

  if (!lpszFileName)
  {
    if (!OnSelectFile(hWnd, hInstance, TRUE, szFileName))
      return FALSE;
  }
  else
  {
    _tcscpy_s(szFileName, lpszFileName);
  }

  UINT nCode = MoveYourApp::GetApp()->GetOptions()->bMI ? 1 : 2;
  if (ListView_GetItemCount(hListhWnd) && !MoveYourApp::GetApp()->GetOptions()->bDNSDWIF)
  {
    ExportImportDlg dlg;
    nCode = dlg.DoImportModal(hWnd);
    if (!nCode || nCode == -1)
      return FALSE;
  }
  if (nCode == 2)
    ListView_DeleteAllItems(hListhWnd);

  HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == hFile)
  {
    ShowOSError(IDS_FILE_OPEN_FAILED, GetLastError());
    return FALSE;
  }

  DWORD dwCheck = 0;
  DWORD dwFileLen = GetFileSize(hFile, &dwCheck);
  if ((dwFileLen > 0xFFFFF) || dwCheck)
  {
    ShowOSError(IDS_SO_LARGE_FILE, GetLastError());
    CloseHandle(hFile);
    return FALSE;
  }
  lpszBuf = new TCHAR[dwFileLen + 1];

  if (!ReadFile(hFile, lpszBuf, dwFileLen, &dwCheck, NULL) && (dwFileLen != dwCheck))
  {
    ShowOSError(IDS_FILE_SYS_ERROR, GetLastError());
    CloseHandle(hFile);
    delete[] lpszBuf;
    return FALSE;
  }
  CloseHandle(hFile);

  lpszBuf[dwFileLen] = 0;
  LPTSTR sz = lpszBuf;
  while (*sz && _istspace(*sz))
  {
    sz = _tcsinc(sz);
  }
  if ((dwFileLen < _tcslen(szSignature) + 2) || (_tcsncmp(szSignature, sz, _tcslen(szSignature)) && !_istspace(*_tcsinc(sz))))
  {
    ShowOSError(IDS_NO_SCRIPT_FILE);
    delete[] lpszBuf;
    return FALSE;
  }
  sz += _tcslen(szSignature) + 1;
  while (*sz)
  {
    while (*sz && _istspace(*sz))
    {
      sz = _tcsinc(sz);
    }
    if (!*sz)
      break;

    LPITEMDATA lpFileData = new ITEMDATA;

    int i;
    for (i = 0; *sz && (*sz != 13) && (*sz != 10) && _tcsncmp(sz, szPointer, _tcslen(szPointer)) && (i < MAX_PATH); i++, sz++)
    {
      lpFileData->szSource[i] = *sz;
    }
    if (!*sz || ((*sz == 13) || (*sz == 10)))
    {
      delete lpFileData;
      continue;
    }
    lpFileData->szSource[i] = 0;
    TrimPath(lpFileData->szSource);
    sz = sz + _tcslen(szPointer);
    for (i = 0; *sz && _tcsncmp(sz, (LPTSTR)szEOL, sizeof(szEOL)) && (i < MAX_PATH); i++, sz++)
    {
      lpFileData->szDestination[i] = *sz;
    }
    if (!*sz)
    {
      delete lpFileData;
      continue;
    }
    lpFileData->szDestination[i] = 0;
    sz = sz + sizeof(szEOL);
    TrimPath(lpFileData->szDestination);
    if (!_tcsicmp(lpFileData->szDestination, szNULL))
      lpFileData->szDestination[0] = 0;

    MoveYourApp::GetApp()->GetMainWnd()->AddToList(lpFileData);
  }
  delete[] lpszBuf;
  return TRUE;
}

BOOL FileData::OnPaste(HWND hWnd, HWND hListView)
{
  BOOL bRet = FALSE;
  if (OpenClipboard(hWnd))
  {
    HGLOBAL hClipData = (HGLOBAL)GetClipboardData(CF_UNICODETEXT);
    if (hClipData)
    {
      int nInsertBefore = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
      LPTSTR lpszClipData = (LPTSTR)GlobalLock(hClipData);
      LPTSTR sz = lpszClipData;
      while (*sz)
      {
        while (*sz && _istspace(*sz))
          sz = _tcsinc(sz);

        if (!*sz)
          break;

        std::unique_ptr<ITEMDATA> lpFileData = std::make_unique<ITEMDATA>();

        int i;
        for (i = 0; *sz && (*sz != 13) && (*sz != 10) && _tcsncmp(sz, szPointer, std::size(szPointer) - 1) && (i < MAX_PATH); i++, sz++)
          lpFileData->szSource[i] = *sz;

        if (!*sz || (*sz == 13) || (*sz == 10))
          continue;

        lpFileData->szSource[i] = 0;
        TrimPath(lpFileData->szSource);

        TCHAR szCheckPointer[std::size(szPointer)];

        //for (i = 0; i < std::size(szPointer) - 1; i++, sz++)
        for (i = 0; *sz && (*sz != 13) && (*sz != 10) && _tcsncmp(sz, szEOL, std::size(szEOL) - 1) && (i < std::size(szPointer) - 1); i++, sz++)
          szCheckPointer[i] = *sz;

        szCheckPointer[i] = 0;

        if (_tcscmp(szPointer, szCheckPointer))
          continue;

        for (i = 0; *sz && (*sz != 13) && (*sz != 10) && _tcsncmp(sz, szEOL, std::size(szEOL) - 1) && (i < MAX_PATH); i++, sz++)
          lpFileData->szDestination[i] = *sz;

        lpFileData->szDestination[i] = 0;
        TrimPath(lpFileData->szDestination);

        if (!_tcsicmp(lpFileData->szDestination, szNULL))
          lpFileData->szDestination[0] = 0;

        if (nInsertBefore != -1)
          MoveYourApp::GetApp()->GetMainWnd()->InsertItem(lpFileData.release(), nInsertBefore);
        else
          MoveYourApp::GetApp()->GetMainWnd()->AddToList(lpFileData.release());
      }
      GlobalUnlock(hClipData);
      bRet = TRUE;
    }
    CloseClipboard();
  }
  return bRet;
}

BOOL FileData::OnCopyCut(HWND hWnd, HWND hListView, BOOL bCut)
{
  BOOL bRet = FALSE;
  if (OpenClipboard(hWnd) && EmptyClipboard())
  {
    ITEMDATA FileData;
    int nIndex = -1;
    HGLOBAL hClipData = NULL;
    LPTSTR lpszClipData = NULL;
    while (MoveYourApp::GetApp()->GetMainWnd()->GetListItemData(&FileData, TRUE, nIndex))
    {
      if (hClipData)
        hClipData = GlobalReAlloc(hClipData, 2 * (MAX_PATH + 10) * sizeof(TCHAR), GHND);
      else
        hClipData = GlobalAlloc(GHND, 2 * (MAX_PATH + 10) * sizeof(TCHAR));

      _ASSERTE(hClipData);
      if (!hClipData)
        break;

      lpszClipData = (LPTSTR)GlobalLock(hClipData);
      _ASSERTE(lpszClipData);
      if (!lpszClipData)
        break;

      if (_tcslen(lpszClipData) > 0)
        _tcscat_s(lpszClipData, _tcslen(lpszClipData) + std::size(szEOL), szEOL);

      _tcscat_s(lpszClipData, _tcslen(lpszClipData) + MAX_PATH, FileData.szSource);
      _tcscat_s(lpszClipData, _tcslen(lpszClipData) + std::size(szPointer), szPointer);

      if (FileData.szDestination[0])
      {
        _tcscat_s(lpszClipData, _tcslen(lpszClipData) + MAX_PATH, FileData.szDestination);
      }
      else
      {
        _tcscat_s(lpszClipData, _tcslen(lpszClipData) + std::size(szNULL), szNULL);
      }

      GlobalUnlock(hClipData);

      if (bCut)
      {
        ListView_DeleteItem(hListView, nIndex);
        nIndex--;
      }
    }

    HANDLE hClip = SetClipboardData(CF_UNICODETEXT, hClipData);
    if (!hClip)
      GlobalFree(hClipData);
    CloseClipboard();
    bRet = bCut;
  }
  return bRet;
}

BOOL FileData::SetEQU(HWND hListhWnd)
{
  ReportBox(IDS_UPDATE_TO_EQU, MB_OK | MB_ICONWARNING);
  ListView_DeleteAllItems(hListhWnd);
  return FillList(hListhWnd);
}
