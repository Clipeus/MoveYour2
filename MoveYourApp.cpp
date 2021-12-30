#define STRICT
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "MoveYourWnd.h"
#include "BrowserCtl.h"
#include "Utils.h"
#include "AppOptions.h"

static TCHAR s_szControl[] = _T("Control Panel\\MMCPL");
static TCHAR s_szMoveCtrl[] = _T("MoveCtrl.cpl");
static TCHAR s_szAppPath[] = _T("AppPath");

LPCTSTR MoveYourApp::s_szMainKey = _T("SOFTWARE\\Mikhail Lyubushkin\\Move Your Files");
MoveYourApp* MoveYourApp::m_pMoveYourApp = nullptr;

MoveYourApp::MoveYourApp()
{

}

MoveYourApp::~MoveYourApp()
{
  ExitMoveYour();
}

MoveYourApp* MoveYourApp::GetApp()
{
  if (m_pMoveYourApp == nullptr)
    m_pMoveYourApp = new MoveYourApp;

  return m_pMoveYourApp;
}

bool MoveYourApp::Init(HINSTANCE hinstExe, LPTSTR pszCmdLine)
{
  HWND hWnd = FindWindow(MoveYourWnd::s_szWindowClass, NULL);
  if (hWnd)
  {
    if (IsIconic(hWnd))
      ShowWindow(hWnd, SW_RESTORE);
    SetForegroundWindow(hWnd);
    return false;
  }

  SetGlobalHandlers();

  m_hInstance = hinstExe;

  if (ParseCommandLine(pszCmdLine))
    return 0;

  INITCOMMONCONTROLSEX iccs = { 0 };
  iccs.dwSize = sizeof(iccs);
  iccs.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&iccs);
  InitBrowserCtl();

  m_pAppOptions = std::make_unique<AppOptions>();
  m_pAppOptions->ReadOptions();

  //m_strAppName.resize(256);
  //LoadString(m_hInstance, IDS_TITLE, &m_strAppName.front(), 256);
  //m_strAppName.shrink_to_fit();
  m_strAppName = LoadString(IDS_TITLE);

  ::GetFixedInfo(m_FixedInfo);

  m_hWaitCursor = LoadCursor(NULL, IDC_WAIT);
  m_hStdCursor = LoadCursor(NULL, IDC_ARROW);

  m_pMainWnd = std::make_unique<MoveYourWnd>();
  return m_pMainWnd->Init();
}

int MoveYourApp::Run(int nCmdShow)
{
  if (!m_pMainWnd->Create(nCmdShow))
    return 1;

  HACCEL hAccel = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCEL));
  if (!hAccel)
  {
    ShowOSError();
    return 1;
  }

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
  {
    if (!TranslateAccelerator(m_pMainWnd.get()->GetHWND(), hAccel, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return msg.wParam;
}

LPCTSTR MoveYourApp::FindOneOf(LPCTSTR p1, LPCTSTR p2) const
{
  while (p1 != NULL && *p1 != NULL)
  {
    LPCTSTR p = p2;
    while (p != NULL && *p != NULL)
    {
      if (*p1 == *p)
        return CharNext(p1);
      p = CharNext(p);
    }
    p1 = CharNext(p1);
  }
  return NULL;
}

void MoveYourApp::DoRegistry(bool bReg) const
{
  if (bReg)
  {
    TCHAR szPath[MAX_PATH + 1];
    if (GetModuleFileName(m_hInstance, szPath, MAX_PATH))
    {
      SetRegString(s_szMainKey, s_szAppPath, szPath, HKEY_LOCAL_MACHINE);
      LPTSTR sz;
      if (sz = _tcsrchr(szPath, _T('\\')))
      {
        *sz = 0;
        _tcscat_s(szPath, _T("\\"));
        _tcscat_s(szPath, s_szMoveCtrl);
        SetRegString(s_szControl, s_szMoveCtrl, szPath, HKEY_CURRENT_USER);
      }
    }
  } else
  {
    RegDeleteKey(HKEY_LOCAL_MACHINE, s_szMainKey);
    RegDeleteKey(HKEY_CURRENT_USER, s_szMainKey);

    HKEY hKey;
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, s_szControl, 0, KEY_ALL_ACCESS, &hKey))
    {
      RegDeleteValue(hKey, s_szMoveCtrl);
      RegCloseKey(hKey);
    }
  }
}

bool MoveYourApp::ParseCommandLine(LPTSTR pszCmdLine) const
{
  bool bSilent = FALSE;

  TCHAR szTokens[] = _T("-/");
  LPCTSTR lpszToken = FindOneOf(pszCmdLine, szTokens);
  while (lpszToken != NULL)
  {
    if (lstrcmpi(lpszToken, _T("UnregServer")) == 0)
    {
      bSilent = true;
      DoRegistry(false);
      break;
    }
    if (lstrcmpi(lpszToken, _T("RegServer")) == 0)
    {
      bSilent = true;
      DoRegistry(true);
      break;
    }
    lpszToken = FindOneOf(lpszToken, szTokens);
  }

  if (!bSilent)
    DoRegistry(true);

  return bSilent;
}

void MoveYourApp::ExitMoveYour()
{
  m_pAppOptions->WriteOptions();
}

void MoveYourApp::Abort()
{
  try
  {
    ExitMoveYour();
    _RPTF0(_CRT_ASSERT, "Abort function called!\n");
  } catch (...)
  {
  }
  exit(-1);
}
