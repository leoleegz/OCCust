#include "TxtHandler.h"
#include "StdAfx.h"

/************************************
  REVISION LOG ENTRY
  Revision By: Zhang, Zhefu
  E-mail: codetiger@hotmail.com
  Revised on 10/2/2003 
  Comment: This is program code accompanying "COM Interface Hooking and Its Application"
           written by Zhefu Zhang posted on www.codeguru.com 
           You are free to reuse the code on the base of keeping this comment
		   All Right Reserved by author		   
 ************************************/

//Rich Edit Stream Out
EDITSTREAM myStream = {
	0,			// dwCookie -- app specific
	0,			// dwError
	NULL		// Callback
};

DWORD CALLBACK writeFunc(
	DWORD_PTR dwCookie, // application-defined value
    LPBYTE pbBuff,      // data buffer
    LONG cb,            // number of bytes to read or write
    LONG *pcb           // number of bytes transferred
)
{
	LPBYTE lpMem = (LPBYTE)(dwCookie);
    LPBYTE lpByte = lpMem;

	DWORD dwSize, dwUsed;
	::CopyMemory(&dwSize, lpByte, sizeof(DWORD));
	lpByte += sizeof(DWORD);
	::CopyMemory(&dwUsed, lpByte, sizeof(DWORD));
    lpByte += sizeof(DWORD);

	lpByte += dwUsed;
	::memcpy(lpByte, pbBuff, cb);
	dwUsed += cb;

	lpByte = (LPBYTE)lpMem;
	lpByte += sizeof(DWORD);
	::CopyMemory(lpByte, &dwUsed, sizeof(DWORD));

	*pcb = cb;
	return 0;
}

//just write to a disk file for now, MMF is used for later case
BOOL GeneralTxtHandler(ITextServices* lpTs)
{
	TCHAR szFilename[MAX_PATH];
	CreateFileName(szFilename, _T("C:\\"), _T(".rtf"));
	
	HANDLE hFile = ::CreateFile(szFilename, GENERIC_WRITE, 0,
	        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DWORD dwWritten;
    //assume the text is less than 1Mb
	LPVOID lpMem = ::VirtualAlloc(NULL, 1024*1024, MEM_COMMIT, PAGE_READWRITE);
	LPBYTE lpByte = (LPBYTE)lpMem;
	DWORD dwSize = 1024 * 1024;
	::CopyMemory(lpByte, &dwSize, sizeof(DWORD));
	lpByte += sizeof(DWORD);
	DWORD dwUsed = 0;
    ::CopyMemory(lpByte, &dwUsed, sizeof(DWORD));
	lpByte = (LPBYTE)lpMem;

	myStream.dwCookie = (DWORD_PTR)lpByte;
	myStream.dwError = 0;
	myStream.pfnCallback = writeFunc;
    PopMsg(_T("Before Stream"));
	LRESULT lr = 0;
	lpTs->TxSendMessage( 
            EM_STREAMOUT,             // message to send
            (WPARAM) (SF_RTF | SF_UNICODE),          // format options
            (LPARAM)(EDITSTREAM*)&myStream,     // data (EDITSTREAM *)
			&lr);
	lpByte = (LPBYTE)lpMem;
	lpByte += sizeof(DWORD);
    ::CopyMemory(&dwUsed, lpByte, sizeof(DWORD));
    lpByte += sizeof(DWORD);
	::WriteFile(hFile, lpByte, dwUsed, &dwWritten, NULL);
	::VirtualFree(lpMem, 0, MEM_RELEASE);
	::CloseHandle(hFile);
    
	return TRUE;
}

