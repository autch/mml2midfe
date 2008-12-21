
#include "stdafx.h"
#include "RedirectStdIO.h"
#include "RunMML2MIDProcs.h"

namespace
{
  struct StdIOBuffer
  {
    CString sStdOut;
    CString sStdErr;
  };
//typedef BOOL (CALLBACK *fnReadStdOutErr)(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead, LPVOID lpUser);
  BOOL CALLBACK ReadStdOutForBanner(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead, LPVOID lpUser)
  {
    StdIOBuffer* buffer = (StdIOBuffer*)lpUser;
    buffer->sStdOut += CString((LPCTSTR)pBuffer, dwBufferSize);
    return TRUE;
  }

  BOOL CALLBACK ReadStdOutForCompile(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead, LPVOID lpUser)
  {
    StdIOBuffer* buffer = (StdIOBuffer*)lpUser;
    buffer->sStdOut += CString((LPCTSTR)pBuffer, dwBufferSize);
    return TRUE;
  }
  BOOL CALLBACK ReadStdErrForCompile(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead, LPVOID lpUser)
  {
    StdIOBuffer* buffer = (StdIOBuffer*)lpUser;
    buffer->sStdErr += CString((LPCTSTR)pBuffer, dwBufferSize);
    return TRUE;
  }
}

BOOL GetMML2MIDBanner(CString sPath, CString& sRet)
{
  CString sStdOut = "", sTemp = "";

  StdIOBuffer buffer;
  sTemp.Format("\"%s\"", sPath);
  DWORD r = RedirectStdIO(sTemp.GetBuffer(20), NULL, ReadStdOutForBanner, NULL, NULL, &buffer, 0, SW_HIDE);

  if(r)
  {
    sStdOut = buffer.sStdOut;
    sStdOut = sStdOut.Left(sStdOut.Find("\x0d\x0a"));

    sRet = sStdOut;
  }
  return r;
}

BOOL RunMML2MID(CString& sCommandLine, CString& sStdOut, CString& sStdErr)
{
  sStdOut = "";
  sStdErr = "";
  StdIOBuffer buffer;
  DWORD r = RedirectStdIO(sCommandLine.GetBuffer(20), NULL, ReadStdOutForCompile, ReadStdErrForCompile, NULL,
                &buffer, 0, SW_HIDE);
  if(r)
  {
    sStdOut = buffer.sStdOut;
    sStdErr = buffer.sStdErr;
  }
  return r;
}
