#include "stdafx.h"
#include "RedirectStdIO.h"

DWORD StdIORedirector::Run(LPCTSTR pszCommandLine, WORD wShowWindow)
{
	DWORD ret = -1;

	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

	// パイプ作りまくり
	StdIn = Pipe(dwBufferSize);
	DuplicateHandle(GetCurrentProcess(), StdIn.hWrite, GetCurrentProcess(), &hStdInWritePipeDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
	StdIn.CloseWrite();

	StdOut = Pipe(dwBufferSize);
	StdErr = Pipe(dwBufferSize);

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = wShowWindow;
	si.hStdInput = StdIn.hRead;
	si.hStdOutput = StdOut.hWrite;
	si.hStdError = StdErr.hWrite;

	PROCESS_INFORMATION pi;
	// れっつ起動
	if (CreateProcess(nullptr, const_cast<LPTSTR>(pszCommandLine), &sa, nullptr,
		TRUE, DETACHED_PROCESS | CREATE_NO_WINDOW,
		nullptr, nullptr, &si, &pi))
	{
		// 入力待ちに入るまで待っててやる
		WaitForInputIdle(pi.hProcess, WAIT_FOR_READY);

		// stdin 不要なら先に閉じてしまう
		if (!pfnWriteStdIn)
			CloseHandle(hStdInWritePipeDup);

		// 転送用バッファ。
		pbyBuffer = std::make_unique<BYTE[]>(dwBufferSize);

		DWORD dwRet;
		do
		{
			// ポンプを動かす
			if (!PumpPipe())
				break;
			dwRet = WaitForSingleObject(pi.hProcess, WAIT_FOR_RUN);
		} while (dwRet != WAIT_OBJECT_0); // ターゲットプロセスが生きている間

		while (true)
		{
			DWORD dwBytesInStdOut = 0, dwBytesInStdErr = 0;
			const BOOL rOut = PeekNamedPipe(StdOut.hRead, nullptr, 0, nullptr, &dwBytesInStdOut, nullptr);
			const BOOL rErr = PeekNamedPipe(StdErr.hRead, nullptr, 0, nullptr, &dwBytesInStdErr, nullptr);
			if ((!rOut && !rErr) || (!dwBytesInStdOut && !dwBytesInStdErr))
				break;
			PumpPipe();             // 書き残し、読み残しはないか？
			Sleep(WAIT_FOR_RUN);
		}

		pbyBuffer.reset();

		// 終了コード
		GetExitCodeProcess(pi.hProcess, &ret);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		CloseHandle(hStdInWritePipeDup);
	}
	return ret;
}

// パイプから読んでコールバックに渡す
BOOL StdIORedirector::ReadFromPipe(HANDLE hPipe, const ReadWriteCallback& pfnReadStdOutErr) const
{
	DWORD dwBytesAvail = 0, dwBytesRead = 0;
	// パイプに何か来てる？
	if (PeekNamedPipe(hPipe, nullptr, 0, nullptr, &dwBytesAvail, nullptr))
	{
		if (!dwBytesAvail) return TRUE; // パイプに何もなし：処理は成功

		// 転送バッファサイズよりも大きいデータが届いた
		if (dwBytesAvail > dwBufferSize) dwBytesAvail = dwBufferSize;

		if (ReadFile(hPipe, pbyBuffer.get(), dwBytesAvail, &dwBytesRead, nullptr))
		{
			if (pfnReadStdOutErr)
				return pfnReadStdOutErr(pbyBuffer.get(), dwBytesRead, &dwBytesRead);
			return TRUE; // コールバックは設定されていないがバッファから吐き出す必要がある
		}
	}
	return FALSE; // パイプを覗けない：エラー
}

// コールバックからもらってパイプに書く
BOOL StdIORedirector::WriteToPipe(HANDLE hPipe, const ReadWriteCallback& pfnWriteStdIn) const
{
	DWORD dwBytesWritten = 0;
	const BOOL bCallbackResult = pfnWriteStdIn(pbyBuffer.get(), dwBufferSize, &dwBytesWritten);

	if (!WriteFile(hPipe, pbyBuffer.get(), dwBytesWritten, &dwBytesWritten, nullptr))
		return FALSE;

	return bCallbackResult;
}

// ポンプを回す
BOOL StdIORedirector::PumpPipe()
{
	if (pfnWriteStdIn)
	{
		// stdin を与える
		if (!WriteToPipe(hStdInWritePipeDup, pfnWriteStdIn))
		{
			// コールバックから FALSE が返ってきた：もう与えるデータはない
			CloseHandle(hStdInWritePipeDup);
			pfnWriteStdIn = nullptr;
		}
	}

	const BOOL rOut = ReadFromPipe(StdOut.hRead, pfnReadStdOut);
	const BOOL rErr = ReadFromPipe(StdErr.hRead, pfnReadStdErr);
	return rOut || rErr;
}
