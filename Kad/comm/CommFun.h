#pragma once
#ifndef _COMMFUN_H_
#define  _COMMFUN_H_
#include "atlenc.h"
#include "../def/types.h"
#include "nb30.h"

class CAICHHash;
struct in_addr;
#define ARRSIZE(x)	(sizeof(x)/sizeof(x[0]))

#ifdef _DEBUG
#define malloc(s)		  _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s)	  _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(p, s)	  _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _expand(p, s)	  _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define free(p)			  _free_dbg(p, _NORMAL_BLOCK)
#define _msize(p)		  _msize_dbg(p, _NORMAL_BLOCK)
#endif
#define ROUND(x) (floor((float)x+0.5f))
#define	SHORT_ED2K_STR			256
#define	SHORT_RAW_ED2K_MB_STR	(SHORT_ED2K_STR*2)
#define	SHORT_RAW_ED2K_UTF8_STR	(SHORT_ED2K_STR*4)

enum EED2KFileType
{
	ED2KFT_ANY				= 0,
	ED2KFT_AUDIO			= 1,	// ED2K protocol value (eserver 17.6+)
	ED2KFT_VIDEO			= 2,	// ED2K protocol value (eserver 17.6+)
	ED2KFT_IMAGE			= 3,	// ED2K protocol value (eserver 17.6+)
	ED2KFT_PROGRAM			= 4,	// ED2K protocol value (eserver 17.6+)
	ED2KFT_DOCUMENT			= 5,	// ED2K protocol value (eserver 17.6+)
	ED2KFT_ARCHIVE			= 6,	// ED2K protocol value (eserver 17.6+)
	ED2KFT_CDIMAGE			= 7,	// ED2K protocol value (eserver 17.6+)
	ED2KFT_EMULECOLLECTION	= 8
};

CString GetFileTypeByName(LPCTSTR pszFileName);
LPCSTR GetED2KFileTypeSearchTerm(EED2KFileType iFileID);
EED2KFileType GetED2KFileTypeSearchID(EED2KFileType iFileID);
EED2KFileType GetED2KFileTypeID(LPCTSTR pszFileName);
int __cdecl CompareE2DKFileType(const void* p1, const void* p2);

void md4str(const uchar* hash, TCHAR* pszHash);
typedef CArray<CStringA> CStringAArray;
typedef CStringArray CStringWArray;

#define _TWINAPI(fname)	fname "W"
#define  _DEL_            0
extern "C" int __cdecl _stricmp(const char * dst, const char * src);

//////////////////////////////////////////////////////////////////////////
__inline void md4cpy(void* dst, const void* src) {
	((uint32*)dst)[0] = ((uint32*)src)[0];
	((uint32*)dst)[1] = ((uint32*)src)[1];
	((uint32*)dst)[2] = ((uint32*)src)[2];
	((uint32*)dst)[3] = ((uint32*)src)[3];
}
__inline int md4cmp(const void* hash1, const void* hash2) {
	return !(((uint32*)hash1)[0] == ((uint32*)hash2)[0] &&
		((uint32*)hash1)[1] == ((uint32*)hash2)[1] &&
		((uint32*)hash1)[2] == ((uint32*)hash2)[2] &&
		((uint32*)hash1)[3] == ((uint32*)hash2)[3]);
}

__inline bool isnulmd4(const void* hash) 
{
	return  (((uint32*)hash)[0] == 0 &&
		((uint32*)hash)[1] == 0 &&
		((uint32*)hash)[2] == 0 &&
		((uint32*)hash)[3] == 0);
}

// md4clr -- replacement for memset(hash,0,16)
__inline void md4clr(const void* hash)
{
	((uint32*)hash)[0] = ((uint32*)hash)[1] = ((uint32*)hash)[2] = ((uint32*)hash)[3] = 0;
}
__inline bool NeedUTF8String(LPCWSTR pwsz)
{
	while (*pwsz != L'\0')
	{
		if (*pwsz > 0x007f/*ATL_ASCII*/)	// VS2008: ATL_ASCII(0x007f) is no longer defined
			return true;
		pwsz++;
	}
	return false;
}
__inline char* nstrdup(const char* todup){
	size_t len = strlen(todup) + 1;
	return (char*)memcpy(new char[len], todup, len);
}

CString ipstr(uint32 nIP);
CString ipstr(uint32 nIP, uint16 nPort);
CString ipstr(LPCTSTR pszAddress, uint16 nPort);
CStringA ipstrA(uint32 nIP);
void ipstrA(CHAR* pszAddress, int iMaxAddress, uint32 nIP);
__inline CString ipstr(in_addr nIP)
{
	return ipstr(*(uint32*)&nIP);
}
__inline CStringA ipstrA(in_addr nIP)
{
	return ipstrA(*(uint32*)&nIP);
}

CStringA wc2utf8(const CStringW& rwstr);
CString OptUtf8ToStr(const CStringA& rastr);
CString OptUtf8ToStr(LPCSTR psz, int iLen);
CString OptUtf8ToStr(const CStringW& rwstr);
CStringA StrToUtf8(const CString& rstr);
CString EncodeUrlUtf8(const CString& rstr);
int utf8towc(LPCSTR pcUtf8, UINT uUtf8Size, LPWSTR pwc, UINT uWideCharSize);
int ByteStreamToWideChar(LPCSTR pcUtf8, UINT uUtf8Size, LPWSTR pwc, UINT uWideCharSize);
CStringW DecodeDoubleEncodedUtf8(LPCWSTR pszFileName);
bool _tmakepathlimit(TCHAR *path, const TCHAR *drive, const TCHAR *dir, const TCHAR *fname, const TCHAR *ext);
void Debug(LPCTSTR pszFmtMsg, ...);
void DbgSetThreadName(LPCSTR szThreadName, ...);
void DebugSend(LPCSTR pszOpcode, uint32 ip, uint16 port);
void DebugSendF(LPCSTR pszOpcode, uint32 ip, uint16 port, LPCTSTR pszMsg, ...);
void DebugRecv(LPCSTR pszOpcode, uint32 ip, uint16 port);

uint16 GetRandomUInt16();
uint32 GetRandomUInt32();
bool IsLANIP(uint32 nIP);
int GetPathDriveNumber(CString path);
bool gotostring(CFile &file, const uchar *find, LONGLONG plen);
bool IsGoodIP(uint32 nIP, bool forceCheck=false);
bool IsGoodIPPort(uint32 nIP, uint16 nPort);
CString md4str(const uchar* hash);
uint32 LevenshteinDistance(const CString& str1, const CString& str2);

struct RC4_Key_Struct{
	uint8 abyState[256];
	uint8 byX;
	uint8 byY;
};

RC4_Key_Struct* RC4CreateKey(const uchar* pachKeyData, uint32 nLen, RC4_Key_Struct* key = NULL, bool bSkipDiscard = false);
void RC4Crypt(const uchar* pachIn, uchar* pachOut, uint32 nLen, RC4_Key_Struct* key);


int CompareLocaleStringNoCase(LPCTSTR psz1, LPCTSTR psz2);
__inline int CompareUnsigned(uint32 uSize1, uint32 uSize2)
{
	if (uSize1 < uSize2)
		return -1;
	if (uSize1 > uSize2)
		return 1;
	return 0;
}
__inline int CompareUnsignedUndefinedAtBottom(uint32 uSize1, uint32 uSize2, bool bSortAscending)
{
	if (uSize1 == 0 && uSize2 == 0)
		return 0;
	if (uSize1 == 0)
		return bSortAscending ? 1 : -1;
	if (uSize2 == 0)
		return bSortAscending ? -1 : 1;
	return CompareUnsigned(uSize1, uSize2);
}

__inline int CompareUnsigned64(uint64 uSize1, uint64 uSize2)
{
	if (uSize1 < uSize2)
		return -1;
	if (uSize1 > uSize2)
		return 1;
	return 0;
}

__inline int CompareFloat(float uSize1, float uSize2)
{
	if (uSize1 < uSize2)
		return -1;
	if (uSize1 > uSize2)
		return 1;
	return 0;
}

__inline int CompareOptLocaleStringNoCase(LPCTSTR psz1, LPCTSTR psz2)
{
	if (psz1 && psz2)
		return CompareLocaleStringNoCase(psz1, psz2);
	if (psz1)
		return -1;
	if (psz2)
		return 1;
	return 0;
}

__inline int CompareOptLocaleStringNoCaseUndefinedAtBottom(const CString &str1, const CString &str2, bool bSortAscending)
{
	if (str1.IsEmpty() && str2.IsEmpty())
		return 0;
	if (str1.IsEmpty())
		return bSortAscending ? 1 : -1;
	if (str2.IsEmpty())
		return bSortAscending ? -1 : 1;
	return CompareOptLocaleStringNoCase(str1, str2);
}
bool strmd4(const char* pszHash, uchar* hash);
bool strmd4(const CString& rstr, uchar* hash);
CString EncodeBase32(const unsigned char* buffer, unsigned int bufLen);
CString EncodeBase16(const unsigned char* buffer, unsigned int bufLen);
unsigned int DecodeLengthBase16(unsigned int base16Length);
bool DecodeBase16(const TCHAR *base16Buffer, unsigned int base16BufLen, byte *buffer, unsigned int bufflen);
uint32 DecodeBase32(LPCTSTR pszInput, uchar* paucOutput, uint32 nBufferLen);
uint32 DecodeBase32(LPCTSTR pszInput, CAICHHash& Hash);
CString GetFormatedUInt64(ULONGLONG ullVal);
CString StripInvalidFilenameChars(const CString& strText);


template< int t_nBufferLength = SHORT_ED2K_STR*4 >
class TUnicodeToUTF8
{
public:
	TUnicodeToUTF8(const CStringW& rwstr)
	{
		int iBuffSize;
		int iMaxEncodedStrSize = rwstr.GetLength()*4;
		if (iMaxEncodedStrSize > t_nBufferLength)
		{
			iBuffSize = iMaxEncodedStrSize;
			m_psz = new char[iBuffSize];
		}
		else
		{
			iBuffSize = ARRSIZE(m_acBuff);
			m_psz = m_acBuff;
		}

		m_iChars = AtlUnicodeToUTF8(rwstr, rwstr.GetLength(), m_psz, iBuffSize);
		ASSERT( m_iChars > 0 || rwstr.GetLength() == 0 );
	}

	TUnicodeToUTF8(LPCWSTR pwsz, int iLength = -1)
	{
		if (iLength == -1)
			iLength = wcslen(pwsz);
		int iBuffSize;
		int iMaxEncodedStrSize = iLength*4;
		if (iMaxEncodedStrSize > t_nBufferLength)
		{
			iBuffSize = iMaxEncodedStrSize;
			m_psz = new char[iBuffSize];
		}
		else
		{
			iBuffSize = ARRSIZE(m_acBuff);
			m_psz = m_acBuff;
		}

		m_iChars = AtlUnicodeToUTF8(pwsz, iLength, m_psz, iBuffSize);
		ASSERT( m_iChars > 0 || iLength == 0 );
	}

	~TUnicodeToUTF8()
	{
		if (m_psz != m_acBuff)
			delete[] m_psz;
	}

	operator LPCSTR() const
	{
		return m_psz;
	}

	int GetLength() const
	{
		return m_iChars;
	}

private:
	int m_iChars;
	LPSTR m_psz;
	char m_acBuff[t_nBufferLength];
};

typedef TUnicodeToUTF8<> CUnicodeToUTF8;


///////////////////////////////////////////////////////////////////////////////
// TUnicodeToBOMUTF8

template< int t_nBufferLength = SHORT_ED2K_STR*4 >
class TUnicodeToBOMUTF8
{
public:
	TUnicodeToBOMUTF8(const CStringW& rwstr)
	{
		int iBuffSize;
		int iMaxEncodedStrSize = 3 + rwstr.GetLength()*4;
		if (iMaxEncodedStrSize > t_nBufferLength)
		{
			iBuffSize = iMaxEncodedStrSize;
			m_psz = new char[iBuffSize];
		}
		else
		{
			iBuffSize = ARRSIZE(m_acBuff);
			m_psz = m_acBuff;
		}

		m_psz[0] = 0xEFU;
		m_psz[1] = 0xBBU;
		m_psz[2] = 0xBFU;
		m_iChars = 3 + AtlUnicodeToUTF8(rwstr, rwstr.GetLength(), m_psz + 3, iBuffSize - 3);
		ASSERT( m_iChars > 3 || rwstr.GetLength() == 0 );
	}

	~TUnicodeToBOMUTF8()
	{
		if (m_psz != m_acBuff)
			delete[] m_psz;
	}

	operator LPCSTR() const
	{
		return m_psz;
	}

	int GetLength() const
	{
		return m_iChars;
	}

private:
	int m_iChars;
	LPSTR m_psz;
	char m_acBuff[t_nBufferLength];
};

typedef TUnicodeToBOMUTF8<> CUnicodeToBOMUTF8;


///////////////////////////////////////////////////////////////////////////////
// TUnicodeToMultiByte

template< int t_nBufferLength = SHORT_ED2K_STR*2 >
class TUnicodeToMultiByte
{
public:
	TUnicodeToMultiByte(const CStringW& rwstr, UINT uCodePage = _AtlGetConversionACP())
	{
		int iBuffSize;
		int iMaxEncodedStrSize = rwstr.GetLength()*2;
		if (iMaxEncodedStrSize > t_nBufferLength)
		{
			iBuffSize = iMaxEncodedStrSize;
			m_psz = new char[iBuffSize];
		}
		else
		{
			iBuffSize = ARRSIZE(m_acBuff);
			m_psz = m_acBuff;
		}

		m_iChars = WideCharToMultiByte(uCodePage, 0, rwstr, rwstr.GetLength(), m_psz, iBuffSize, NULL, 0);
		ASSERT( m_iChars > 0 || rwstr.GetLength() == 0 );
	}

	~TUnicodeToMultiByte()
	{
		if (m_psz != m_acBuff)
			delete[] m_psz;
	}

	operator LPCSTR() const
	{
		return m_psz;
	}

	int GetLength() const
	{
		return m_iChars;
	}

private:
	int m_iChars;
	LPSTR m_psz;
	char m_acBuff[t_nBufferLength];
};

typedef TUnicodeToMultiByte<> CUnicodeToMultiByte;


CString CastItoXBytes(uint16 count, bool isK = false, bool isPerSec = false, uint32 decimal = 2);
CString CastItoXBytes(uint32 count, bool isK = false, bool isPerSec = false, uint32 decimal = 2);
CString CastItoXBytes(uint64 count, bool isK = false, bool isPerSec = false, uint32 decimal = 2);
CString CastItoXBytes(float count, bool isK = false, bool isPerSec = false, uint32 decimal = 2);
CString CastItoXBytes(double count, bool isK = false, bool isPerSec = false, uint32 decimal = 2);

//×´Ì¬ÐÅÏ¢
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//Íø¿¨×´Ì¬
	NAME_BUFFER						NameBuff[16];						//Ãû×Ö»º³å
};


#define  LEN_NETWORK_ID 13
//////////////////////////////////////////////////////////////////////////
BOOL GetMACAddress(TCHAR szMACAddress[LEN_NETWORK_ID]);
CString GetWindowVersion();

#endif