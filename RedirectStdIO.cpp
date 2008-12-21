#include "stdafx.h"

// ���J�֐��̐����̓w�b�_�ɂ���
#include "RedirectStdIO.h"

// �f�t�H���g�o�b�t�@�T�C�Y
#define BUFFER_SIZE 8192
// ���͑҂���ԂɂȂ�̂�҂���
#define WAIT_FOR_READY 1000
// stdin/stdout/stderr �����󂷂郋�[�v�̒��ł̃E�F�C�g
#define WAIT_FOR_RUN 1

// �p�C�v�n���h���̃��b�p
typedef struct tagPipe
{
  HANDLE hRead;
  HANDLE hWrite;
}Pipe;

// �֐������ŗ��p����R���e�L�X�g
typedef struct tagRedirStdIOContext
{
  Pipe StdIn;
  Pipe StdOut;
  Pipe StdErr;
  HANDLE hStdInWritePipeDup; // stdin ��^����Ƃ��͂����ɏ�������

  fnWriteStdIn pfnWriteStdIn;
  fnReadStdOutErr pfnReadStdOut;
  fnReadStdOutErr pfnReadStdErr;
  LPVOID lpUser;

  BYTE* pbyBuffer;           // �]���o�b�t�@
  DWORD dwBufferSize;        // �o�b�t�@�T�C�Y
}RedirStdIOContext;

// stdin/stdout/stderr ���R�[���o�b�N���Ăяo���ď�������B
BOOL PumpPipe(RedirStdIOContext* pContext);
// hPipe �̃p�C�v�� pfnWriteStdIn ���瓾���f�[�^����������
BOOL WriteToPipe(RedirStdIOContext* pContext, HANDLE hPipe, fnWriteStdIn pfnWriteStdIn);
// hPipe ���瓾���f�[�^�� pfnReadStdOutErr �֓n��
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
  // �f�t�H���g�o�b�t�@�T�C�Y
  if(context.dwBufferSize <= 0) context.dwBufferSize = BUFFER_SIZE;

  // �p�C�v���܂���
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
  // ����N��
  BOOL r = CreateProcess(NULL, pszCommandLine, &sa, NULL, TRUE, DETACHED_PROCESS | CREATE_NO_WINDOW,
                         NULL, NULL, &si, &pi);
  if(r)
  {
    // ���͑҂��ɓ���܂ő҂��ĂĂ��
    WaitForInputIdle(pi.hProcess, WAIT_FOR_READY);

    // stdin �s�v�Ȃ��ɕ��Ă��܂�
    if(!context.pfnWriteStdIn)
      CloseHandle(context.hStdInWritePipeDup);

    // �]���p�o�b�t�@�B
    context.pbyBuffer = new BYTE[context.dwBufferSize];

    DWORD dwRet;
    do
    {
      // �|���v�𓮂���
      if(!PumpPipe(&context))
        break;
      dwRet = WaitForSingleObject(pi.hProcess, WAIT_FOR_RUN);
    }while(dwRet != WAIT_OBJECT_0); // �^�[�Q�b�g�v���Z�X�������Ă����

    DWORD dwBytesInStdOut = 0, dwBytesInStdErr = 0;
    while(1)
    {
      BOOL r;
      r = PeekNamedPipe(context.StdOut.hRead, NULL, 0, NULL, &dwBytesInStdOut, NULL);
      r = r && PeekNamedPipe(context.StdErr.hRead, NULL, 0, NULL, &dwBytesInStdErr, NULL);
      if(!r || (!dwBytesInStdOut && !dwBytesInStdErr))
        break;
      PumpPipe(&context);             // �����c���A�ǂݎc���͂Ȃ����H
      Sleep(WAIT_FOR_RUN);
    }

    delete[] context.pbyBuffer;

    // �I���R�[�h
    if(pdwReturnCode) GetExitCodeProcess(pi.hProcess, pdwReturnCode);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
  }
  // ��n��
  CloseHandle(context.StdIn.hRead);
  CloseHandle(context.StdOut.hRead); CloseHandle(context.StdOut.hWrite);
  CloseHandle(context.StdErr.hRead); CloseHandle(context.StdErr.hWrite);

  return r;
}

// �p�C�v����ǂ�ŃR�[���o�b�N�ɓn��
BOOL ReadFromPipe(RedirStdIOContext* pContext, HANDLE hPipe, fnReadStdOutErr pfnReadStdOutErr)
{
  DWORD dwBytesAvail = 0, dwBytesRead = 0;
  // �p�C�v�ɉ������Ă�H
  if(PeekNamedPipe(hPipe, NULL, 0, NULL, &dwBytesAvail, NULL))
  {
    if(!dwBytesAvail) return TRUE; // �p�C�v�ɉ����Ȃ��F�����͐���

    // �]���o�b�t�@�T�C�Y�����傫���f�[�^���͂���
    if(dwBytesAvail > pContext->dwBufferSize) dwBytesAvail = pContext->dwBufferSize;

    if(ReadFile(hPipe, pContext->pbyBuffer, dwBytesAvail, &dwBytesRead, NULL))
    {
      if(pfnReadStdOutErr)
        return pfnReadStdOutErr(pContext->pbyBuffer, dwBytesRead, &dwBytesRead, pContext->lpUser);
      return TRUE; // �R�[���o�b�N�͐ݒ肳��Ă��Ȃ����o�b�t�@����f���o���K�v������
    }
  }
  return FALSE; // �p�C�v��`���Ȃ��F�G���[
}

// �R�[���o�b�N���������ăp�C�v�ɏ���
BOOL WriteToPipe(RedirStdIOContext* pContext, HANDLE hPipe, fnWriteStdIn pfnWriteStdIn)
{
  DWORD dwBytesWritten = 0;
  BOOL bCallbackResult = pfnWriteStdIn(pContext->pbyBuffer, pContext->dwBufferSize, &dwBytesWritten, pContext->lpUser);

  if(!WriteFile(hPipe, pContext->pbyBuffer, dwBytesWritten, &dwBytesWritten, NULL))
    return FALSE;

  return bCallbackResult;
}

// �|���v����
BOOL PumpPipe(RedirStdIOContext* pContext)
{
  BOOL r = TRUE;
  if(pContext->pfnWriteStdIn)
  {
    // stdin ��^����
    if(!WriteToPipe(pContext, pContext->hStdInWritePipeDup, pContext->pfnWriteStdIn))
    {
      // �R�[���o�b�N���� FALSE ���Ԃ��Ă����F�����^����f�[�^�͂Ȃ�
      CloseHandle(pContext->hStdInWritePipeDup);
      pContext->pfnWriteStdIn = NULL;
    }
  }

  // �ǂ��炩�̃R�[���o�b�N�������f�[�^�͂����ƌ����܂�
  r = r && ReadFromPipe(pContext, pContext->StdOut.hRead, pContext->pfnReadStdOut);
  r = r && ReadFromPipe(pContext, pContext->StdErr.hRead, pContext->pfnReadStdErr);
  return r;
}
