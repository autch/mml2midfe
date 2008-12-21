/*
 * RedirectStdIO.h
 *   コンソールリダイレクション
 */
#if !defined(REDIRECTSTDIO_H)
#define REDIRECTSTDIO_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "user32.lib")

// ターゲットアプリの stdin として与えるデータを pBuffer へコピーする。
// out LPVOID pBuffer          ターゲットの stdin として与えたいデータをここへ
//                             コピーするためのバッファ。
// in  DWORD dwBufferSize      pBuffer のサイズ。
// out DWORD* pdwBytesWritten  pBuffer に実際に書き込んだバイト数を返す。
// i/o LPVOID lpUser           RedirectStdIO() に渡した lpUser.  用途は任意。
// ret BOOL                    続きのデータがあるとき TRUE
typedef BOOL (CALLBACK *fnWriteStdIn)(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesWritten, LPVOID lpUser);

// ターゲットアプリが吐いた stdout や stderr を pBuffer から受け取る
// in  LPVOID pBuffer          ターゲットアプリが吐いた stdout/stderr のデータ
// in  DWORD dwBufferSize      pBuffer に実際に入っているバイト数。
// out DWORD* pdwBytesRead     pBuffer から読み取ったバイト数を返す。
// i/o LPVOID lpUser           RedirectStdIO() に渡した lpUser.  用途は任意。
// ret BOOL                    もっとデータがほしければ TRUE
typedef BOOL (CALLBACK *fnReadStdOutErr)(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead, LPVOID lpUser);

// コンソールアプリの標準入出力を横取りする（元には返さない）
DWORD RedirectStdIO(LPSTR pszCommandLine,         // コマンドライン
                    fnWriteStdIn pfnWriteStdIn,   // stdin を与えるコールバック
                    fnReadStdOutErr pfnReadStdOut,// stdout を受け取る callback
                    fnReadStdOutErr pfnReadStdErr,// stderr を受け取る callback
                    DWORD* pdwReturnCode,         // コンソールアプリの戻り値
                    LPVOID lpUser,                // callback へ渡す任意の値
                    DWORD dwBufferSize,           // 初期バッファサイズ。
                                                  // 0 でデフォルト
                    WORD wShowWindow = SW_SHOW    // ウィンドウの表示の仕方。
                                                  // ShowWindow() の引数。
                   );

// おまけ：Win32 のファイルハンドルから stdio の FILE* を得る。
// mode は fopen() の mode 引数。
FILE* GetStdioStreamFromWin32Handle(HANDLE hWin32Handle, char* mode);

#endif // !REDIRECTSTDIO_H
