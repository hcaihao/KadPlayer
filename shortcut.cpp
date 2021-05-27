#include "stdafx.h"
#include <shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>

namespace Shortcut
{
BOOL GetDesktopPath(char *desktopPath)
{
	LPITEMIDLIST  ppidl = NULL; 

	if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &ppidl) == S_OK)
	{
		BOOL flag =	SHGetPathFromIDListA(ppidl, desktopPath);
		CoTaskMemFree(ppidl);
		return flag;
	}

	return FALSE;
}

BOOL GetIEQuickLaunchPath(char *iEQueickLaunchPath)
{
	LPITEMIDLIST  ppidl; 			

	if (SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &ppidl) == S_OK)
	{
		BOOL flag =	SHGetPathFromIDListA(ppidl, iEQueickLaunchPath);
		strcat(iEQueickLaunchPath, "\\Microsoft\\Internet Explorer\\Quick Launch");
		CoTaskMemFree(ppidl);
		return flag;
	}

	return FALSE;
}

BOOL GetProgramsPath(char *programsPath)
{
	LPITEMIDLIST  ppidl; 

	if (SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAMS, &ppidl) == S_OK)
	{
		BOOL flag = SHGetPathFromIDListA(ppidl, programsPath);
		CoTaskMemFree(ppidl);
		return flag;
	}

	return FALSE;
}

BOOL CreateFileShortcut(char *lpszFileName, 
	char *lpszLnkFileDir, 
	char *lpszLnkFileName,	
	char *lpszWorkDir, 
	WORD wHotkey, 
	char *lpszDescription, 
	int iShowCmd = SW_SHOWNORMAL)
{
	if (lpszLnkFileDir == NULL)
		return FALSE;

	HRESULT hr;
	IShellLinkA     *pLink;  //IShellLink对象指针
	IPersistFile   *ppf; //IPersisFil对象指针

	//创建IShellLink对象
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//从IShellLink对象中获取IPersistFile接口
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	//目标
	if (lpszFileName == NULL)
		pLink->SetPath(_pgmptr);
	else
		pLink->SetPath(lpszFileName);

	//工作目录
	if (lpszWorkDir != NULL)
		pLink->SetPath(lpszWorkDir);

	//快捷键
	if (wHotkey != 0)
		pLink->SetHotkey(wHotkey);

	//备注
	if (lpszDescription != NULL)
		pLink->SetDescription(lpszDescription);

	//显示方式
	pLink->SetShowCmd(iShowCmd);


	//快捷方式的路径 + 名称
	char szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //指定了快捷方式的名称
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, lpszLnkFileName);
	else   
	{
		//没有指定名称，就从取指定文件的文件名作为快捷方式名称。
		const char *pstr;
		if (lpszFileName != NULL)
			pstr = strrchr(lpszFileName, '\\');
		else
			pstr = strrchr(_pgmptr, '\\');

		if (pstr == NULL)
		{	
			ppf->Release();
			pLink->Release();
			return FALSE;
		}
		//注意后缀名要从.exe改为.lnk
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, pstr);
		int nLen = strlen(szBuffer);
		szBuffer[nLen - 3] = 'l';
		szBuffer[nLen - 2] = 'n';
		szBuffer[nLen - 1] = 'k';
	}
	//保存快捷方式到指定目录下
	WCHAR  wsz[MAX_PATH];  //定义Unicode字符串
	MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

	hr = ppf->Save(wsz, TRUE);

	ppf->Release();
	pLink->Release();
	return SUCCEEDED(hr);
}

BOOL AutoCreateCurrentFileShortcut()
{
	char path[MAX_PATH] = {0};
	CoInitialize(NULL);

	GetDesktopPath(path);
	BOOL f = CreateFileShortcut(NULL, 
		path, 
		NULL, 
		NULL, 
		NULL, 
		NULL
		);

	CoUninitialize();
	return f;
}

}// namespace Shortcut