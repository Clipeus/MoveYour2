#pragma once

#include <memory>
#include "MoveYour.h"

class MoveYourWnd;
struct AppOptions;

class MoveYourApp
{
  MoveYourApp();

public:
  ~MoveYourApp();
  static MoveYourApp* GetApp();

public:
  bool Init(HINSTANCE hinstExe, LPTSTR pszCmdLine);
  int Run(int nCmdShow);
  void Abort();

public:
  HINSTANCE GetInstance() const
  {
    return m_hInstance;
  }
  MoveYourWnd* GetMainWnd() const
  {
    return m_pMainWnd.get();
  }
  AppOptions* GetOptions() const
  {
    return m_pAppOptions.get();
  }
  LPCTSTR GetAppName() const
  {
    return m_strAppName.c_str();
  }
  bool IsWaitCursor() const
  {
    return m_bWaitCursor;
  }
  void SetWaitCursor(bool bWait)
  {
    m_bWaitCursor = bWait;
  }
  HCURSOR GetWaitCursor() const
  {
    return m_hWaitCursor;
  }
  HCURSOR GetStdCursor() const
  {
    return m_hStdCursor;
  }
  const VS_FIXEDFILEINFO& GetFixedInfo() const
  {
    return m_FixedInfo;
  }

private:
  LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2) const;
  void DoRegistry(bool bReg) const;
  bool ParseCommandLine(LPTSTR pszCmdLine) const;
  void ExitMoveYour();

public:
  static LPCTSTR s_szMainKey;

private:
  static MoveYourApp* m_pMoveYourApp;

  tstring m_strAppName;
  HINSTANCE m_hInstance = nullptr;
  std::unique_ptr<MoveYourWnd> m_pMainWnd;
  std::unique_ptr<AppOptions> m_pAppOptions;
  bool m_bWaitCursor = false;
  HCURSOR m_hWaitCursor = nullptr;
  HCURSOR m_hStdCursor = nullptr;
  VS_FIXEDFILEINFO m_FixedInfo = { 0 };

};

