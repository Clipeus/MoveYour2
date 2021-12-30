/*
    Author: Mikhail Lyubushkin

    Project:		Move Your Files 2.0
    Description:	Error handles
*/

#include <Windows.h>
#include <tchar.h>
#include "MoveYour.h"
#include "MoveYourApp.h"
#include "Utils.h"
#include <eh.h>
#include <new.h>

LONG WINAPI UnExFilter(EXCEPTION_POINTERS* pExceptionInfo)
{
  //FatalAppExit(0, LoadString(IDS_UNEXCEPTION).c_str());
  ReportBox(IDS_UNEXCEPTION, MB_OK | MB_TASKMODAL | MB_ICONSTOP);
  MoveYourApp::GetApp()->Abort();
  return EXCEPTION_CONTINUE_SEARCH;
}

void _thread_se_translator(unsigned int nCode, EXCEPTION_POINTERS* pException)
{
  _RPTF1(_CRT_ERROR, "SEH -> Exception code : %X\n", pException->ExceptionRecord->ExceptionCode);
  throw(pException);
}

int _handle_out_of_memory(size_t nSize)
{
  _RPTF1(_CRT_ERROR, "NEW -> Out of memory for %d bytes\n", nSize);
  if (IDCANCEL == ReportBox(IDS_OUT_OF_MEM, MB_RETRYCANCEL | MB_TASKMODAL | MB_ICONSTOP))
  {
    MoveYourApp::GetApp()->Abort();
    return 0;
  }
  return 1;
}

void SetGlobalHandlers()
{
  SetUnhandledExceptionFilter(UnExFilter);	// For Win32 SEH
  _set_se_translator(_thread_se_translator);	// For C++ try/catch
  _set_new_handler(_handle_out_of_memory);
  _set_new_mode(1);
}
