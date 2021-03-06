#pragma once
#include "CInclude.h"

#include "CMyFile.h"
#include "CPE.h"
#include "CMyProcess.h"
#include "CMyHwnd.h"
#include "CMyServer.h"
#include "CHeap.h"


#define defExitID程序正常退出			0x0000;
#define defExitID无法解析大文件		0x00F1


extern "C" _declspec(dllexport)
DWORD	GetMd5_ByCertutil(PTCHAR pPath, char md5[33]);
extern "C" _declspec(dllexport)
void*	GetFilePtr(char* pPath, __int64& pSize, char* strSize = nullptr);

extern double FILETIME2Double(const _FILETIME& fileTime);

extern "C" _declspec(dllexport)
float GetCpuUsage();


extern "C" _declspec(dllexport)
BOOL EnableDebugPrivilege();


typedef void(*FILECALLBACK)(LPFILEINFO);
typedef struct  _CallEnumFilePaths {
    TCHAR Path[MAX_PATH];
    FILECALLBACK CallAddress;
}CallEnumFilePaths, * LPCallEnumFilePaths;

extern _declspec(dllexport)
BOOL CALLBACK EnumFilePaths(void(*CALLBACKPROC)(LPFILEINFO),
	PTCHAR Path, CHeap* heap, PTCHAR Kind = nullptr);


extern _declspec(dllexport)
BOOL CALLBACK API_LoadDll(HANDLE hProcess, PTCHAR DllPath);

//extern _declspec(dllexport)
//BOOL CALLBACK API_FreeDll(HANDLE hProcess);



class CApi
{
};

