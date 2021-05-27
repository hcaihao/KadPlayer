//this file is part of eMule
//Copyright (C)2002-2008 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once
#include "../def/types.h"
#include "../comm/CommFun.h"

#define	PARTMET_BAK_EXT	_T(".bak")
#define	PARTMET_TMP_EXT	_T(".backup")

#define PR_VERYLOW			4 // I Had to change this because it didn't save negative number correctly.. Had to modify the sort function for this change..
#define PR_LOW				0 //*
#define PR_NORMAL			1 // Don't change this - needed for edonkey clients and server!
#define PR_HIGH				2 //*
#define PR_VERYHIGH			3
#define PR_AUTO				5 //UAP Hunter

const CString strDefaultToolbar = _T("0099010203040506070899091011");

enum EViewSharedFilesAccess{
	vsfaEverybody = 0,
	vsfaFriends = 1,
	vsfaNobody = 2
};

enum ENotifierSoundType{
	ntfstNoSound = 0,
	ntfstSoundFile = 1,
	ntfstSpeech = 2
};

enum EDefaultDirectory{
	EMULE_CONFIGDIR = 0,
	EMULE_TEMPDIR = 1,
	EMULE_INCOMINGDIR = 2,
	EMULE_LOGDIR = 3,
	EMULE_ADDLANGDIR = 4, // directories with languages installed by the eMule (parent: EMULE_EXPANSIONDIR)
	EMULE_INSTLANGDIR = 5, // directories with languages installed by the user or installer (parent: EMULE_EXECUTEABLEDIR)
	EMULE_WEBSERVERDIR = 6,
	EMULE_SKINDIR = 7,
	EMULE_DATABASEDIR = 8, // the parent directory of the incoming/temp folder
	EMULE_CONFIGBASEDIR = 9, // the parent directory of the config folder 
	EMULE_EXECUTEABLEDIR = 10, // assumed to be not writeable (!)
	EMULE_TOOLBARDIR = 11,
	EMULE_EXPANSIONDIR = 12 // this is a base directory accessable for all users for things eMule installs
};


enum EToolbarLabelType;
enum ELogFileFormat;

// DO NOT EDIT VALUES like making a uint16 to uint32, or insert any value. ONLY append new vars
#pragma pack(1)
struct Preferences_Ext_Struct{
	uint8	version;
	uchar	userhash[16];
	WINDOWPLACEMENT EmuleWindowPlacement;
};
#pragma pack()


struct ProxySettings{
	uint16		type;
	uint16		port;
	CStringA	name;
	CStringA	user;
	CStringA	password;
	bool		EnablePassword;
	bool		UseProxy;
};

struct Category_Struct{
	CString	strIncomingPath;
	CString	strTitle;
	CString	strComment;
	DWORD	color;
	UINT	prio;
	CString autocat;
	CString	regexp;
	int		filter;
	bool	filterNeg;
	bool	care4all;
	bool	ac_regexpeval;
	bool	downloadInAlphabeticalOrder; // ZZ:DownloadManager
};


class   CPreferences
{
public:
	CPreferences();
	~CPreferences();
private:
	static	int		m_iDebugClientKadUDPLevel;
	static  bool	filterLANIPs;
	static  uchar	userhash[16];
	static  Preferences_Ext_Struct* prefsExt;
	static  uint8   smartidstate;
	static  WINDOWPLACEMENT EmuleWindowPlacement;
	static  uint16	 udpport;
	static  uint32	 m_dwKadUDPKey;;
	static  bool	m_bVerbose;
	static  bool	m_bFullVerbose;
	static  int		m_iDebugServerUDPLevel;
	static  bool	m_bCryptLayerSupported;
	static  UINT	filterlevel;
	static LPCSTR	m_pszBindAddrA;
	static CStringA m_strBindAddrA;
	static LPCWSTR	m_pszBindAddrW;
	static CStringW m_strBindAddrW;
	static bool	    m_bLogFilteredIPs;
	static uint16   port;
	static CString	commentFilter;
	static	bool	networkkademlia;
	static	CString	m_strYourHostname;
public:
	static void    Init();
    static int     GetDebugClientKadUDPLevel();
	static bool    FilterLANIPs();
	static uchar*  GetUserHash();
	static uint8   GetSmartIdState();
	static void	   SetSmartIdState(uint8 in_smartidstate);
	static uint16  GetUDPPort();
	static uint16  GetRandomUDPPort();
	static uint32	GetKadUDPKey();
	static	bool	GetLogFilteredIPs()	;
	static  int		GetDebugServerUDPLevel();
	static  bool   IsServerCryptLayerUDPEnabled();
	static  bool	IsClientCryptLayerSupported();
	static  int	   GetIPFilterLevel();
	static  bool	GetVerbose();
	static  LPCWSTR	GetBindAddrW();
	static  uint16	GetPort();
	static	const CString& GetYourHostname()			{return m_strYourHostname;}
	static  const CString& GetCommentFilter();
	static bool	CPreferences::GetNetworkKademlia();

	static CString GetMePath();
	static CString GetConfigPath();
	static CString GetDataPath();
};
extern CPreferences thePrefs;
