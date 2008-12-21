#include "stdafx.h"

// 公開関数の説明はヘッダにある
#include "RedirectStdIO.h"

// デフォルトバッファサイズ
#define BUFFER_SIZE 8192
// 入力待ち状態になるのを待つ時間
#define WAIT_FOR_READY 1000
// stdin/stdout/stderr を授受するループの中でのウェイト
#define WAIT_FOR_RUN 1

// パイプハンドルのラッパ
typedef struct tagPipe
{
  HANDLE hRead;
  HANDLE hWrite;
}Pipe;

// 関数内部で利用するコンテキスト
typedef struct tagRedirStdIOContext
{
  Pipe StdIn;
  Pipe StdOut;
  Pipe StdErr;
  HANDLE hStdInWritePipeDup; // stdin を与えるときはこいつに書き込む

  fnWriteStdIn pfnWriteStdIn;
  fnReadStdOutErr pfnReadStdOut;
  fnReadStdOutErr pfnReadStdErr;
  LPVOID lpUser;

  BYTE* pbyBuffer;           // 転送バッファ
  DWORD dwBufferSize;        // バッファサイズ
}RedirStdIOContext;

// stdin/stdout/stderr をコールバックを呼び出して処理する。
BOOL PumpPipe(RedirStdIOContext* pContext);
// hPipe のパイプへ pfnWriteStdIn から得たデータを書き込む
BOOL WriteToPipe(RedirStdIOContext* pContext, HANDLE hPipe, fnWriteStdIn pfnWriteStdIn);
// hPipe から得たデータを pfnReadStdOutErr へ渡す
BOOL ReadFromPipe(RedirStdIOContext* pContext, HANDLE hPipe, fnReadStdOutErr pfnReadStdOutErr);

DWORD RedirectStdIO(LPSTR pszCommandLine,
                    fnWriteStdIn pfnWriteStdIn,
                    fnReadStdOutErr pfnReadStdOut,
                    fnReadStdOutErr pfnReadStdErr,
                    DWORD* pdwReturnCode,
                    LPVOID lpUser,
                    DWORD dwDefaultBufferSize,
                    WORD wShowWindow
                   )
{
  RedirStdIOContext context;
  ZeroMemory(&context, sizeof(RedirStdIOContext));

  context.pfnWriteStdIn = pfnWriteStdIn;
  context.pfnReadStdOut = pfnReadStdOut;
  context.pfnReadStdErr = pfnReadStdErr;
  context.lpUser = lpUser;

  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

  context.dwBufferSize = dwDefaultBufferSize;
  // デフォルトバッファサイズ
  if(context.dwBufferSize <= 0) context.dwBufferSize = BUFFER_SIZE;

  // パイプ作りまくり
  CreatePipe(&context.StdIn.hRead, &context.StdIn.hWrite, &sa, context.dwBufferSize);
  DuplicateHandle(GetCurrentProcess(), context.StdIn.hWrite, GetCurrentProcess(), &context.hStdInWritePipeDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
  CloseHandle(context.StdIn.hWrite);
  CreatePipe(&context.StdOut.hRead, &context.StdOut.hWrite, &sa, context.dwBufferSize);
  CreatePipe(&context.StdErr.hRead, &context.StdErr.hWrite, &sa, context.dwBufferSize);

  STARTUPINFO si = { sizeof(STARTUPINFO) };

  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = wShowWindow;
  si.hStdInput = context.StdIn.hRead;
  si.hStdOutput = context.StdOut.hWrite;
  si.hStdError = context.StdErr.hWrite;

  PROCESS_INFORMATION pi;
  // れっつ起動
  BOOL r = CreateProcess(NULL, pszCommandLine, &sa, NULL, TRUE, DETACHED_PROCESS | CREATE_NO_WINDOW,
                         NULL, NULL, &si, &pi);
  if(r)
  {
    // 入力待ちに入るまで待っててやる
    WaitForInputIdle(pi.hProcess, WAIT_FOR_READY);

    // stdin 不要なら先に閉じてしまう
    if(!context.pfnWriteStdIn)
      CloseHandle(context.hStdInWritePipeDup);

    // 転送用バッファ。
    context.pbyBuffer = new BYTE[context.dwBufferSize];

    DWORD dwRet;
    do
    {
      // ポンプを動かす
      if(!PumpPipe(&context))
        break;
      dwRet = WaitForSingleObject(pi.hProcess, WAIT_FOR_RUN);
    }while(dwRet != WAIT_OBJECT_0); // ターゲットプロセスが生きている間

    DWORD dwBytesInStdOut = 0, dwBytesInStdErr = 0;
    while(1)
    {
      BOOL r;
      r = PeekNamedPipe(context.StdOut.hRead, NULL, 0, NULL, &dwBytesInStdOut, NULL);
      r = r && PeekNamedPipe(context.StdErr.hRead, NULL, 0, NULL, &dwBytesInStdErr, NULL);
      if(!r || (!dwBytesInStdOut && !dwBytesInStdErr))
        break;
      PumpPipe(&context);             // 書き残し、読み残しはないか？
      Sleep(WAIT_FOR_RUN);
    }

    delete[] context.pbyBuffer;

    // 終了コード
    if(pdwReturnCode) GetExitCodeProcess(pi.hProcess, pdwReturnCode);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
  }
  // 後始末
  CloseHandle(context.StdIn.hRead);
  CloseHandle(context.StdOut.hRead); CloseHandle(context.StdOut.hWrite);
  CloseHandle(context.StdErr.hRead); CloseHandle(context.StdErr.hWrite);

  return r;
}

// パイプから読んでコールバックに渡す
BOOL ReadFromPipe(RedirStdIOContext* pContext, HANDLE hPipe, fnReadStdOutErr pfnReadStdOutErr)
{
  DWORD dwBytesAvail = 0, dwBytesRead = 0;
  // パイプに何か来てる？
  if(PeekNamedPipe(hPipe, NULL, 0, NULL, &dwBytesAvail, NULL))
  {
    if(!dwBytesAvail) return TRUE; // パイプに何もなし：処理は成功

    // 転送バッファサイズよりも大きいデータが届いた
    if(dwBytesAvail > pContext->dwBufferSize) dwBytesAvail = pContext->dwBufferSize;

    if(ReadFile(hPipe, pContext->pbyBuffer, dwBytesAvail, &dwBytesRead, NULL))
    {
      if(pfnReadStdOutErr)
        return pfnReadStdOutErr(pContext->pbyBuffer, dwBytesRead, &dwBytesRead, pContext->lpUser);
      return TRUE; // コールバックは設定されていないがバッファから吐き出す必要がある
    }
  }
  return FALSE; // パイプを覗けない：エラー
}

// コールバックからもらってパイプに書く
BOOL WriteToPipe(RedirStdIOContext* pContext, HANDLE hPipe, fnWriteStdIn pfnWriteStdIn)
{
  DWORD dwBytesWritten = 0;
  BOOL bCallbackResult = pfnWriteStdIn(pContext->pbyBuffer, pContext->dwBufferSize, &dwBytesWritten, pContext->lpUser);

  if(!WriteFile(hPipe, pContext->pbyBuffer, dwBytesWritten, &dwBytesWritten, NULL))
    return FALSE;

  return bCallbackResult;
}

// ポンプを回す
BOOL PumpPipe(RedirStdIOContext* pContext)
{
  BOOL r = TRUE;
  if(pContext->pfnWriteStdIn)
  {
    // stdin を与える
    if(!WriteToPipe(pContext, pContext->hStdInWritePipeDup, pContext->pfnWriteStdIn))
    {
      // コールバックから FALSE が返ってきた：もう与えるデータはない
      CloseHandle(pContext->hStdInWritePipeDup);
      pContext->pfnWriteStdIn = NULL;
    }
  }

  // どちらかのコールバックがもうデータはいらんと言うまで
  r = r && ReadFromPipe(pContext, pContext->StdOut.hRead, pContext->pfnReadStdOut);
  r = r && ReadFromPipe(pContext, pContext->StdErr.hRead, pContext->pfnReadStdErr);
  return r;
}
