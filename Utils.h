/*
		Author: Mikhail Lyubushkin

		Project:		Move Your Files 2.0
		Description:	Utils, helper
*/

#include <crtdbg.h>
#include <memory>

// The normal HANDLE_MSG macro in WINDOWSX.H does not work properly for dialog
// boxes because DlgProc's return a BOOL instead of an LRESULT (like
// WndProcs). This HANDLE_DLGMSG macro corrects the problem:
#define HANDLE_DLGMSG(hWnd, message, fn)                          \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,               \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

#define UNIQUE_VALUE(NAME, TYPE, DELETER) \
using unique_##NAME = std::unique_ptr<std::remove_pointer<TYPE>::type, decltype(&DELETER)>; \
inline unique_##NAME make_unique_##NAME(TYPE value = nullptr) \
{ \
    return unique_##NAME(value, DELETER); \
}

#define UNIQUE_PTR(NAME, PTR, DELETER) \
using unique_##NAME = std::unique_ptr<PTR, decltype(&DELETER)>; \
inline unique_##NAME make_unique_##NAME(PTR* ptr = nullptr) \
{ \
    return unique_##NAME(ptr, DELETER); \
}

UNIQUE_VALUE(handle, HANDLE, ::CloseHandle)
UNIQUE_VALUE(regkey, HKEY, ::RegCloseKey)

void SetGlobalHandlers();

DWORD GetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwDefault = 0, HKEY hRoot = HKEY_CURRENT_USER);
BOOL GetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszDefault, LPTSTR lpszReturnedString, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);
BOOL GetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);
BOOL GetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpDefault, LPVOID lpBuf, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);

BOOL SetRegDWORD(LPCTSTR lpszFolder, LPCTSTR lpszKey, DWORD dwData, HKEY hRoot = HKEY_CURRENT_USER);
BOOL SetRegString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPCTSTR lpszString, HKEY hRoot = HKEY_CURRENT_USER);
BOOL SetRegBinary(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);
BOOL SetRegMString(LPCTSTR lpszFolder, LPCTSTR lpszKey, LPVOID lpBuf, DWORD dwSize, HKEY hRoot = HKEY_CURRENT_USER);

BOOL DeleteRegKey(LPCTSTR lpszFolder, LPCTSTR lpszKey, HKEY hRoot = HKEY_CURRENT_USER);

BOOL GetFixedInfo(VS_FIXEDFILEINFO & rFixedInfo);
tstring LoadString(UINT uID);

int ShowOSError(UINT uMesID = IDS_SYSTEM_ERROR, DWORD dwError = 0, int nMode = MB_OK | MB_ICONERROR);
int ReportBox(LPCTSTR lpszMessage, int nMode = MB_OK);
int ReportBox(UINT uMesID, int nMode = MB_OK);

inline void ScreenToClient(HWND hParentWnd, LPRECT lpRect)
{
	ScreenToClient(hParentWnd, (LPPOINT)lpRect);
	ScreenToClient(hParentWnd, ((LPPOINT)lpRect) + 1);
}

class WaitCursor
{
public:
	WaitCursor();
	~WaitCursor();

protected:
	HCURSOR m_hPrevCursor = nullptr;
};
