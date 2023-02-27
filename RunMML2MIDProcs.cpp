#include "stdafx.h"
#include "RedirectStdIO.h"
#include "RunMML2MIDProcs.h"

namespace
{
	struct StdIOBuffer
	{
		std::vector<BYTE> sStdOut;
		std::vector<BYTE> sStdErr;
	};
}

BOOL GetMML2MIDBanner(CString sPath, CString& sRet)
{
	CString sTemp = "";

	StdIOBuffer buffer;
	auto fnReadStdout = [&buffer](LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead) -> BOOL {
		buffer.sStdOut.insert(buffer.sStdOut.end(), static_cast<BYTE*>(pBuffer), static_cast<BYTE*>(pBuffer) + dwBufferSize);
		*pdwBytesRead = dwBufferSize;
		return TRUE;
	};
	sTemp.Format(_T("\"%s\""), sPath);

	StdIORedirector redir(nullptr, fnReadStdout, nullptr);
	BOOL r = redir.Run(sTemp.GetBuffer(20)) != -1;
	if (r)
	{
		CString sStdOut = CString(buffer.sStdOut.data());
		sStdOut = sStdOut.Left(sStdOut.Find(_T("\x0d\x0a")));

		sRet = sStdOut;
	}
	return r;
}

BOOL RunMML2MID(CString& sCommandLine, CString& sStdOut, CString& sStdErr)
{
	sStdOut = "";
	sStdErr = "";
	StdIOBuffer buffer;
	auto fnReadStdout = [&buffer](LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead)->BOOL {
		buffer.sStdOut.insert(buffer.sStdOut.end(), static_cast<BYTE*>(pBuffer), static_cast<BYTE*>(pBuffer) + dwBufferSize);
		*pdwBytesRead = dwBufferSize;
		return TRUE;
	};
	auto fnReadStderr = [&buffer](LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead)->BOOL {
		buffer.sStdErr.insert(buffer.sStdErr.end(), static_cast<BYTE*>(pBuffer), static_cast<BYTE*>(pBuffer) + dwBufferSize);
		*pdwBytesRead = dwBufferSize;
		return TRUE;
	};

	StdIORedirector redir(nullptr, fnReadStdout, fnReadStderr);
	const DWORD r = redir.Run(sCommandLine.GetBuffer(20)) != -1;
	if (r)
	{
		auto bytes2str = [](const auto& vec) -> CString
		{
			const std::string s(std::begin(vec), std::end(vec));
			const CA2W ca2w(s.c_str());
			return { static_cast<LPWSTR>(ca2w) };
		};

		sStdOut = bytes2str(buffer.sStdOut);
		sStdErr = bytes2str(buffer.sStdErr);
	}
	return r;
}
