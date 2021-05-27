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
	IShellLinkA     *pLink;  //IShellLink����ָ��
	IPersistFile   *ppf; //IPersisFil����ָ��

	//����IShellLink����
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//��IShellLink�����л�ȡIPersistFile�ӿ�
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	//Ŀ��
	if (lpszFileName == NULL)
		pLink->SetPath(_pgmptr);
	else
		pLink->SetPath(lpszFileName);

	//����Ŀ¼
	if (lpszWorkDir != NULL)
		pLink->SetPath(lpszWorkDir);

	//��ݼ�
	if (wHotkey != 0)
		pLink->SetHotkey(wHotkey);

	//��ע
	if (lpszDescription != NULL)
		pLink->SetDescription(lpszDescription);

	//��ʾ��ʽ
	pLink->SetShowCmd(iShowCmd);


	//��ݷ�ʽ��·�� + ����
	char szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //ָ���˿�ݷ�ʽ������
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, lpszLnkFileName);
	else   
	{
		//û��ָ�����ƣ��ʹ�ȡָ���ļ����ļ�����Ϊ��ݷ�ʽ���ơ�
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
		//ע���׺��Ҫ��.exe��Ϊ.lnk
		sprintf(szBuffer, "%s\\%s", lpszLnkFileDir, pstr);
		int nLen = strlen(szBuffer);
		szBuffer[nLen - 3] = 'l';
		szBuffer[nLen - 2] = 'n';
		szBuffer[nLen - 1] = 'k';
	}
	//�����ݷ�ʽ��ָ��Ŀ¼��
	WCHAR  wsz[MAX_PATH];  //����Unicode�ַ���
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