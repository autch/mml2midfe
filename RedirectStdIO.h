/*
 * RedirectStdIO.h
 *   �R���\�[�����_�C���N�V����
 */
#if !defined(REDIRECTSTDIO_H)
#define REDIRECTSTDIO_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "user32.lib")

// �^�[�Q�b�g�A�v���� stdin �Ƃ��ė^����f�[�^�� pBuffer �փR�s�[����B
// out LPVOID pBuffer          �^�[�Q�b�g�� stdin �Ƃ��ė^�������f�[�^��������
//                             �R�s�[���邽�߂̃o�b�t�@�B
// in  DWORD dwBufferSize      pBuffer �̃T�C�Y�B
// out DWORD* pdwBytesWritten  pBuffer �Ɏ��ۂɏ������񂾃o�C�g����Ԃ��B
// i/o LPVOID lpUser           RedirectStdIO() �ɓn���� lpUser.  �p�r�͔C�ӁB
// ret BOOL                    �����̃f�[�^������Ƃ� TRUE
typedef BOOL (CALLBACK *fnWriteStdIn)(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesWritten, LPVOID lpUser);

// �^�[�Q�b�g�A�v�����f���� stdout �� stderr �� pBuffer ����󂯎��
// in  LPVOID pBuffer          �^�[�Q�b�g�A�v�����f���� stdout/stderr �̃f�[�^
// in  DWORD dwBufferSize      pBuffer �Ɏ��ۂɓ����Ă���o�C�g���B
// out DWORD* pdwBytesRead     pBuffer ����ǂݎ�����o�C�g����Ԃ��B
// i/o LPVOID lpUser           RedirectStdIO() �ɓn���� lpUser.  �p�r�͔C�ӁB
// ret BOOL                    �����ƃf�[�^���ق������ TRUE
typedef BOOL (CALLBACK *fnReadStdOutErr)(LPVOID pBuffer, DWORD dwBufferSize, DWORD* pdwBytesRead, LPVOID lpUser);

// �R���\�[���A�v���̕W�����o�͂�����肷��i���ɂ͕Ԃ��Ȃ��j
DWORD RedirectStdIO(LPSTR pszCommandLine,         // �R�}���h���C��
                    fnWriteStdIn pfnWriteStdIn,   // stdin ��^����R�[���o�b�N
                    fnReadStdOutErr pfnReadStdOut,// stdout ���󂯎�� callback
                    fnReadStdOutErr pfnReadStdErr,// stderr ���󂯎�� callback
                    DWORD* pdwReturnCode,         // �R���\�[���A�v���̖߂�l
                    LPVOID lpUser,                // callback �֓n���C�ӂ̒l
                    DWORD dwBufferSize,           // �����o�b�t�@�T�C�Y�B
                                                  // 0 �Ńf�t�H���g
                    WORD wShowWindow = SW_SHOW    // �E�B���h�E�̕\���̎d���B
                                                  // ShowWindow() �̈����B
                   );

// ���܂��FWin32 �̃t�@�C���n���h������ stdio �� FILE* �𓾂�B
// mode �� fopen() �� mode �����B
FILE* GetStdioStreamFromWin32Handle(HANDLE hWin32Handle, char* mode);

#endif // !REDIRECTSTDIO_H
