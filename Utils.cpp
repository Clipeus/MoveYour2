/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Utils, helper
*/

#define STRICT
#include <Windows.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "MoveYourWnd.h"
#include "Utils.h"

DWORD GetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwDefault /*= 0*/, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hKey;
  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
  {
    DWORD dwTemp2, dwTemp = sizeof(DWORD);
    DWORD dwRealType;
    if (RegQueryValueEx(hKey, lpszKey, 0, &dwRealType, (LPBYTE)&dwTemp2, &dwTemp) == ERROR_SUCCESS)
    {
      if (REG_DWORD == dwRealType)
      {
        dwDefault = dwTemp2;
      }
    }
    RegCloseKey(hKey);
  }
  return dwDefault;
}

BOOL GetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszDefault, LPTSTR lpszReturnedString, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hKey;
  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
  {
    DWORD dwRealType;
    if (RegQueryValueEx(hKey, lpszKey, 0, &dwRealType, (LPBYTE)lpszReturnedString, &dwSize) != ERROR_SUCCESS)
    {
      RegCloseKey(hKey);
      _tcscpy_s(lpszReturnedString, dwSize / sizeof(TCHAR), lpszDefault);
      return FALSE;
    }
    RegCloseKey(hKey);
    if (REG_SZ == dwRealType)
      return TRUE;
  }
  return FALSE;
}

BOOL GetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  BOOL bRet = FALSE;
  HKEY hKey;
  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
  {
    DWORD dwRealType;
    if (RegQueryValueEx(hKey, lpszKey, 0, &dwRealType, (LPBYTE)lpBuf, &dwSize) == ERROR_SUCCESS)
    {
      if (REG_BINARY == dwRealType)
      {
        bRet = TRUE;
      }
    }
    RegCloseKey(hKey);
  }
  return bRet;
}

BOOL GetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpDefault, LPVOID lpBuf, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hKey;
  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
  {
    DWORD dwRealType;
    if (RegQueryValueEx(hKey, lpszKey, 0, &dwRealType, (LPBYTE)lpBuf, &dwSize) != ERROR_SUCCESS)
    {
      int error = GetLastError();
      RegCloseKey(hKey);
      memmove(lpBuf, lpDefault, dwSize);
      SetLastError(error);
      return FALSE;
    }
    RegCloseKey(hKey);
    if (REG_MULTI_SZ == dwRealType)
      return TRUE;
  }
  return FALSE;
}

BOOL SetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwData, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  BOOL bRet = FALSE;
  HKEY hKey;
  DWORD dwDisp;
  if (RegCreateKeyEx(hRoot, lpszFolder, 0, NULL,
    REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
  {
    if (RegSetValueEx(hKey, lpszKey, 0, REG_DWORD, (CONST BYTE*) & dwData, sizeof(DWORD)) == ERROR_SUCCESS)
    {
      bRet = TRUE;
    }
    RegCloseKey(hKey);
  }
  return bRet;
}

BOOL SetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszString, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  BOOL bRet = FALSE;
  HKEY hKey;
  DWORD dwDisp;
  if (RegCreateKeyEx(hRoot, lpszFolder, 0, NULL,
    REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
  {
    if (RegSetValueEx(hKey, lpszKey, 0, REG_SZ, (CONST BYTE*)lpszString, _tcslen(lpszString)) == ERROR_SUCCESS)
    {
      bRet = TRUE;
    }
    RegCloseKey(hKey);
  }
  return bRet;
}

BOOL SetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  BOOL bRet = FALSE;
  HKEY hKey;
  DWORD dwDisp;
  if (RegCreateKeyEx(hRoot, lpszFolder, 0, NULL,
    REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
  {
    if (RegSetValueEx(hKey, lpszKey, 0, REG_BINARY, (CONST BYTE*)lpBuf, dwSize) == ERROR_SUCCESS)
    {
      bRet = TRUE;
    }
    RegCloseKey(hKey);
  }
  return bRet;
}

BOOL DeleteRegKey(LPCTSTR lpszFolder, LPCTSTR lpszKey, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hKey = NULL;
  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
  {
    if (RegDeleteValue(hKey, lpszKey) == ERROR_SUCCESS)
    {
      RegCloseKey(hKey);
      return TRUE;
    }
    int error = GetLastError();
    RegCloseKey(hKey);
    SetLastError(error);
  }
  return FALSE;
}

BOOL SetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot /*= HKEY_CURRENT_USER*/)
{
  HKEY hKey = NULL;
  if (RegOpenKeyEx(hRoot, lpszFolder, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
  {
    if (RegSetValueEx(hKey, lpszKey, 0, REG_MULTI_SZ, (LPBYTE)lpBuf, dwSize) == ERROR_SUCCESS)
    {
      RegCloseKey(hKey);
      return TRUE;
    }
    int error = GetLastError();
    RegCloseKey(hKey);
    SetLastError(error);
  }
  return FALSE;
}

/* Message helper */
int ReportBox(LPCTSTR lpszMessage, int nMode /*= MB_OK*/)
{
  return MessageBox(MoveYourApp::GetApp()->GetMainWnd()->GetHWND(), lpszMessage, MoveYourApp::GetApp()->GetAppName(), nMode);
}

int ReportBox(UINT uMesID, int nMode /*= MB_OK*/)
{
  return ReportBox(LoadString(uMesID).c_str(), nMode);
}

int ShowOSError(UINT uMesID /*= IDS_SYSTEM_ERROR*/, DWORD dwError /*= 0*/, int nMode /*= MB_OK|MB_ICONERROR*/)
{
  LPTSTR szErrorText = nullptr;

  if (!dwError)
    dwError = ::GetLastError();
  
  if (dwError)
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&szErrorText), 0, nullptr);

  tstring strText = LoadString(uMesID);

  if (szErrorText)
  {
    strText += szErrorText;
    LocalFree(szErrorText);
  }

  return ReportBox(strText.c_str(), nMode);
}

tstring LoadString(UINT uID)
{
  tstring result;
  LPTSTR lpszText = nullptr;
  
  int nLen = ::LoadString(MoveYourApp::GetApp()->GetInstance(), uID, reinterpret_cast<LPTSTR>(&lpszText), 0);

  if (lpszText)
    result.assign(lpszText, nLen);

  return result;
}

BOOL GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
  // get the filename of the executable containing the version resource
  TCHAR szFilename[MAX_PATH + 1] = { 0 };
  if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
  {
    _RPTFN(_CRT_ERROR, "GetModuleFileName failed with error %d\n", GetLastError());
    return FALSE;
  }

  // allocate a block of memory for the version info
  DWORD dummy;
  DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
  if (dwSize == 0)
  {
    _RPTFN(_CRT_ERROR, "GetFileVersionInfoSize failed with error %d\n", GetLastError());
    return FALSE;
  }
  std::vector<BYTE> data;
  data.resize(dwSize);

  // load the version info
  if (!GetFileVersionInfo(szFilename, NULL, dwSize, data.data()))
  {
    _RPTFN(_CRT_ERROR, "GetFileVersionInfo failed with error %d\n", GetLastError());
    return FALSE;
  }

  VS_FIXEDFILEINFO* pFixedInfo;
  UINT uiVerLen = 0;
  if (VerQueryValue(data.data(), TEXT("\\"), (void**)&pFixedInfo, (UINT*)&uiVerLen) == 0)
  {
    _RPTFN(_CRT_ERROR, "VerQueryValue failed with error %d\n", GetLastError());
    return FALSE;
  }

  rFixedInfo = *pFixedInfo;
  return TRUE;
}

WaitCursor::WaitCursor()
{
  m_hPrevCursor = SetCursor(MoveYourApp::GetApp()->GetWaitCursor());
  MoveYourApp::GetApp()->SetWaitCursor(true);
}

WaitCursor::~WaitCursor()
{
  SetCursor(m_hPrevCursor ? m_hPrevCursor : MoveYourApp::GetApp()->GetStdCursor());
  MoveYourApp::GetApp()->SetWaitCursor(false);
}
