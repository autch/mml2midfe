/*
 * RedirectStdIO.h
 *   コンソールリダイレクション
 */
#if !defined(REDIRECTSTDIO_H)
#define REDIRECTSTDIO_H

#pragma comment(lib, "user32.lib")

class StdIORedirector
{
public:
	using ReadWriteCallback = std::function<BOOL(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesReadOrWritten)>;

	StdIORedirector() = default;

	explicit StdIORedirector(
		ReadWriteCallback writeStdIn,
		ReadWriteCallback readStdOut,
		ReadWriteCallback readStdErr,
		DWORD dwBufferSize = BUFFER_SIZE
	) : pfnWriteStdIn(std::move(writeStdIn)), pfnReadStdOut(std::move(readStdOut)), pfnReadStdErr(std::move(readStdErr)), dwBufferSize(dwBufferSize)
	{

	}

	DWORD Run(LPCTSTR szCommandLine, WORD wShowWindow = SW_HIDE);


private:
	// デフォルトバッファサイズ
	static constexpr int BUFFER_SIZE = 8192;
	// 入力待ち状態になるのを待つ時間
	static constexpr int WAIT_FOR_READY = 1000;
	// stdin/stdout/stderr を授受するループの中でのウェイト
	static constexpr int WAIT_FOR_RUN = 20;

	struct Pipe
	{
		HANDLE hRead{ INVALID_HANDLE_VALUE };
		HANDLE hWrite{ INVALID_HANDLE_VALUE };

		Pipe() = default;
		Pipe(HANDLE hR, HANDLE hW) : hRead(hR), hWrite(hW)
		{
		}
		explicit Pipe(DWORD dwBufferSize, BOOL bInheritHandle = TRUE)
		{
			SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, bInheritHandle };
			::CreatePipe(&hRead, &hWrite, &sa, dwBufferSize);
		}
		Pipe(const Pipe& other) = delete;
		Pipe& operator=(const Pipe& other) = delete;

		Pipe(Pipe&& other) noexcept
		{
			*this = std::move(other);
		}

		Pipe& operator=(Pipe&& other) noexcept
		{
			if (this == &other)
				return *this;

			CloseRead();
			hRead = other.hRead;
			CloseWrite();
			hWrite = other.hWrite;

			other.hRead = other.hWrite = INVALID_HANDLE_VALUE;
			return *this;
		}

		virtual ~Pipe()
		{
			CloseRead();
			CloseWrite();
		}

		void CloseRead()
		{
			if (hRead != INVALID_HANDLE_VALUE)
				::CloseHandle(hRead);
			hRead = INVALID_HANDLE_VALUE;
		}
		void CloseWrite()
		{
			if (hWrite != INVALID_HANDLE_VALUE)
				::CloseHandle(hWrite);
			hWrite = INVALID_HANDLE_VALUE;
		}
	};

	Pipe StdIn, StdOut, StdErr;
	HANDLE hStdInWritePipeDup{ INVALID_HANDLE_VALUE }; // stdin を与えるときはこいつに書き込む

	ReadWriteCallback pfnWriteStdIn, pfnReadStdOut, pfnReadStdErr;

	std::unique_ptr<BYTE[]> pbyBuffer;           // 転送バッファ
	DWORD dwBufferSize{ BUFFER_SIZE };        // バッファサイズ


	BOOL PumpPipe();
	BOOL WriteToPipe(HANDLE hPipe, const ReadWriteCallback& pfnWriteStdIn) const;
	BOOL ReadFromPipe(HANDLE hPipe, const ReadWriteCallback& pfnReadStdOutErr) const;
};


// おまけ：Win32 のファイルハンドルから stdio の FILE* を得る。
// mode は fopen() の mode 引数。
FILE* GetStdioStreamFromWin32Handle(HANDLE hWin32Handle, char* mode);

#endif // !REDIRECTSTDIO_H
