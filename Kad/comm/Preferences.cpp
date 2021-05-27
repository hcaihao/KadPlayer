//this file is part of eMule
//Copyright (C)2002-2005 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "stdafx.h"
#include <io.h>
#include <share.h>
#include <iphlpapi.h>
#include "Preferences.h"
#include "../def/opcodes.h"
#include "../comm/commfun.h"
#include "../encry/md5/MD5Sum.h"
#include "../utils/SafeFile.h"
#include "../comm/Log.h"


#pragma warning(disable:4516) // access-declarations are deprecated; member using-declarations provide a better alternative
#pragma warning(disable:4244) // conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4100) // unreferenced formal parameter
#pragma warning(disable:4702) // unreachable code
#include <osrng.h>
#pragma warning(default:4702) // unreachable code
#pragma warning(default:4100) // unreferenced formal parameter
#pragma warning(default:4244) // conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default:4516) // access-declarations are deprecated; member using-declarations provide a better alternative


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPreferences thePrefs;

int		CPreferences::m_iDebugClientKadUDPLevel;
bool	CPreferences::filterLANIPs;
uchar	CPreferences::userhash[16];
Preferences_Ext_Struct* CPreferences::prefsExt;
uint8 CPreferences::smartidstate;
WINDOWPLACEMENT CPreferences::EmuleWindowPlacement;
uint16	CPreferences::udpport;
uint32	CPreferences::m_dwKadUDPKey;
bool	CPreferences::m_bVerbose;
bool	CPreferences::m_bFullVerbose;
int		CPreferences::m_iDebugServerUDPLevel;
bool	CPreferences::m_bCryptLayerSupported;
UINT	CPreferences::filterlevel;
LPCSTR	CPreferences::m_pszBindAddrA;
CStringA CPreferences::m_strBindAddrA;
LPCWSTR	CPreferences::m_pszBindAddrW;
CStringW CPreferences::m_strBindAddrW;
bool	CPreferences::m_bLogFilteredIPs;
uint16 CPreferences::port;
CString	CPreferences::commentFilter;
bool	CPreferences::networkkademlia;
CString	CPreferences::m_strYourHostname;

CPreferences::CPreferences()
{
}

CPreferences::~CPreferences()
{
	delete prefsExt;
}

void CPreferences::Init()
{
	srand((uint32)time(0)); // we need random numbers sometimes

	prefsExt = new Preferences_Ext_Struct;
	memset(prefsExt, 0, sizeof *prefsExt);

	m_iDebugClientKadUDPLevel = 0;
	filterLANIPs =true;

	CString strFullPath;
	strFullPath =GetConfigPath() + L"preferences.dat";
	FILE* preffile = _tfsopen(strFullPath, L"rb", _SH_DENYWR);

	//LoadPreferences();

	if (!preffile){
		//SetStandartValues();
	}
	else{
		if (fread(prefsExt,sizeof(Preferences_Ext_Struct),1,preffile) != 1 || ferror(preffile))
		//	SetStandartValues();

		md4cpy(userhash, prefsExt->userhash);
		EmuleWindowPlacement = prefsExt->EmuleWindowPlacement;

		fclose(preffile);
		smartidstate = 0;
	}

	udpport = thePrefs.GetRandomUDPPort();
	m_dwKadUDPKey = GetRandomUInt32();
	m_iDebugServerUDPLevel = 0;
	m_bCryptLayerSupported = true;
	filterlevel = 127;


	m_strBindAddrW = _T("");
	m_strBindAddrW.Trim();
	m_pszBindAddrW = m_strBindAddrW.IsEmpty() ? NULL : (LPCWSTR)m_strBindAddrW;
	m_strBindAddrA = m_strBindAddrW;
	m_pszBindAddrA = m_strBindAddrA.IsEmpty() ? NULL : (LPCSTR)m_strBindAddrA;

	m_bLogFilteredIPs =true;
	port = 24640;
	commentFilter =L"http://|https://|ftp://|www.|ftp.";
	networkkademlia =TRUE;
	m_strYourHostname =_T("");

}
int	CPreferences::GetDebugClientKadUDPLevel()
{
	return m_iDebugClientKadUDPLevel;
}
bool   CPreferences::FilterLANIPs()
{
	return filterLANIPs;
}
uchar*	CPreferences::GetUserHash()
{
	return userhash;
}
uint8	CPreferences::GetSmartIdState()			
{
	return smartidstate;
}
void	CPreferences::SetSmartIdState(uint8 in_smartidstate)
{
	smartidstate = in_smartidstate;
}
uint16	CPreferences::GetUDPPort()
{
	return udpport;
}
uint32	CPreferences::GetKadUDPKey()
{
	return m_dwKadUDPKey;
}
uint16 CPreferences::GetRandomUDPPort()
{
	// Get table of currently used UDP ports.
	PMIB_UDPTABLE pUDPTab = NULL;
	// Couple of crash dmp files are showing that we may crash somewhere in 'iphlpapi.dll' when doing the 2nd call
	__try {
		HMODULE hIpHlpDll = LoadLibrary(_T("iphlpapi.dll"));
		if (hIpHlpDll)
		{
			DWORD (WINAPI *pfnGetUdpTable)(PMIB_UDPTABLE, PDWORD, BOOL);
			(FARPROC&)pfnGetUdpTable = GetProcAddress(hIpHlpDll, "GetUdpTable");
			if (pfnGetUdpTable)
			{
				DWORD dwSize = 0;
				if ((*pfnGetUdpTable)(NULL, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
				{
					// The nr. of UDP entries could change (increase) between
					// the two function calls, allocate some more memory.
					dwSize += sizeof(pUDPTab->table[0]) * 50;
					pUDPTab = (PMIB_UDPTABLE)malloc(dwSize);
					if (pUDPTab)
					{
						if ((*pfnGetUdpTable)(pUDPTab, &dwSize, TRUE) != ERROR_SUCCESS)
						{
							free(pUDPTab);
							pUDPTab = NULL;
						}
					}
				}
			}
			FreeLibrary(hIpHlpDll);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		free(pUDPTab);
		pUDPTab = NULL;
	}

	const UINT uValidPortRange = 61000;
	int iMaxTests = uValidPortRange; // just in case, avoid endless loop
	uint16 nPort;
	bool bPortIsFree;
	do {
		// Get random port
		nPort = 4096 + (GetRandomUInt16() % uValidPortRange);

		// The port is by default assumed to be available. If we got a table of currently
		// used UDP ports, we verify that this port is currently not used in any way.
		bPortIsFree = true;
		if (pUDPTab)
		{
			uint16 nPortBE = htons(nPort);
			for (UINT e = 0; e < pUDPTab->dwNumEntries; e++)
			{
				if (pUDPTab->table[e].dwLocalPort == nPortBE)
				{
					bPortIsFree = false;
					break;
				}
			}
		}
	}
	while (!bPortIsFree && --iMaxTests > 0);
	free(pUDPTab);
	return nPort;
}
bool	CPreferences::GetLogFilteredIPs()	
{
	return m_bVerbose && m_bLogFilteredIPs;
}

int	 CPreferences::GetDebugServerUDPLevel() 
{
	return m_iDebugServerUDPLevel;
}
bool	CPreferences::IsClientCryptLayerSupported()		
{
	return m_bCryptLayerSupported;
}
bool CPreferences::IsServerCryptLayerUDPEnabled()
{
	return IsClientCryptLayerSupported();
}

int	CPreferences::GetIPFilterLevel()	
{
	return filterlevel;
}


CString CPreferences::GetMePath()
{
	TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL,buffer,MAX_PATH);
	LPTSTR pszFileName = _tcsrchr(buffer, L'\\') + 1;
	*pszFileName = L'\0';
	return CString(buffer);
}
CString CPreferences::GetConfigPath()
{
	CString strConfigPath = _T("");
	strConfigPath.Format(_T("%s\data\\"),GetMePath());
	return strConfigPath;
}
CString CPreferences::GetDataPath()
{
	/*
	CString strDataPath = _T("");
	strDataPath.Format(_T("%s\data\\"),GetMePath());
	return strDataPath;
	*/
	return GetConfigPath();
}
bool	CPreferences::GetVerbose()
{
	return m_bVerbose;
}
LPCWSTR	CPreferences::GetBindAddrW()					
{
	return m_pszBindAddrW; 
}
uint16	CPreferences::GetPort()	
{
	return port;
}
const CString& CPreferences::GetCommentFilter()		
{
	return commentFilter;
}
bool	CPreferences::GetNetworkKademlia()				{return networkkademlia && udpport > 0;}