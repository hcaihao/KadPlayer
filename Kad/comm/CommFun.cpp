
#include "stdafx.h"
#include "atlenc.h"
#include "CommFun.h"
#include "../encry/hashencry/SHAHashSet.h"
#include "../def/opcodes.h"



// Base chars for encode an decode functions
static byte base16Chars[17] = "0123456789ABCDEF";
static byte base32Chars[33] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
#define BASE16_LOOKUP_MAX 23
static byte base16Lookup[BASE16_LOOKUP_MAX][2] = {
	{ '0', 0x0 },
	{ '1', 0x1 },
	{ '2', 0x2 },
	{ '3', 0x3 },
	{ '4', 0x4 },
	{ '5', 0x5 },
	{ '6', 0x6 },
	{ '7', 0x7 },
	{ '8', 0x8 },
	{ '9', 0x9 },
	{ ':', 0x9 },
	{ ';', 0x9 },
	{ '<', 0x9 },
	{ '=', 0x9 },
	{ '>', 0x9 },
	{ '?', 0x9 },
	{ '@', 0x9 },
	{ 'A', 0xA },
	{ 'B', 0xB },
	{ 'C', 0xC },
	{ 'D', 0xD },
	{ 'E', 0xE },
	{ 'F', 0xF }
};

struct SED2KFileType
{
	LPCTSTR pszExt;
	EED2KFileType iFileType;
} g_aED2KFileTypes[] = 
{
	{ _T(".aac"),   ED2KFT_AUDIO },     // Advanced Audio Coding File
	{ _T(".ac3"),   ED2KFT_AUDIO },     // Audio Codec 3 File
	{ _T(".aif"),   ED2KFT_AUDIO },     // Audio Interchange File Format
	{ _T(".aifc"),  ED2KFT_AUDIO },     // Audio Interchange File Format
	{ _T(".aiff"),  ED2KFT_AUDIO },     // Audio Interchange File Format
	{ _T(".amr"),   ED2KFT_AUDIO },     // Adaptive Multi-Rate Codec File
	{ _T(".ape"),   ED2KFT_AUDIO },     // Monkey's Audio Lossless Audio File
	{ _T(".au"),    ED2KFT_AUDIO },     // Audio File (Sun, Unix)
	{ _T(".aud"),   ED2KFT_AUDIO },     // General Audio File
	{ _T(".audio"), ED2KFT_AUDIO },     // General Audio File
	{ _T(".cda"),   ED2KFT_AUDIO },     // CD Audio Track
	{ _T(".dmf"),   ED2KFT_AUDIO },     // Delusion Digital Music File
	{ _T(".dsm"),   ED2KFT_AUDIO },     // Digital Sound Module
	{ _T(".dts"),   ED2KFT_AUDIO },     // DTS Encoded Audio File
	{ _T(".far"),   ED2KFT_AUDIO },     // Farandole Composer Module
	{ _T(".flac"),  ED2KFT_AUDIO },     // Free Lossless Audio Codec File
	{ _T(".it"),    ED2KFT_AUDIO },     // Impulse Tracker Module
	{ _T(".m1a"),   ED2KFT_AUDIO },     // MPEG-1 Audio File
	{ _T(".m2a"),   ED2KFT_AUDIO },     // MPEG-2 Audio File
	{ _T(".m4a"),   ED2KFT_AUDIO },     // MPEG-4 Audio File
	{ _T(".mdl"),   ED2KFT_AUDIO },     // DigiTrakker Module
	{ _T(".med"),   ED2KFT_AUDIO },     // Amiga MED Sound File
	{ _T(".mid"),   ED2KFT_AUDIO },     // MIDI File
	{ _T(".midi"),  ED2KFT_AUDIO },     // MIDI File
	{ _T(".mka"),   ED2KFT_AUDIO },     // Matroska Audio File
	{ _T(".mod"),   ED2KFT_AUDIO },     // Amiga Music Module File
	{ _T(".mp1"),   ED2KFT_AUDIO },     // MPEG-1 Audio File
	{ _T(".mp2"),   ED2KFT_AUDIO },     // MPEG-2 Audio File
	{ _T(".mp3"),   ED2KFT_AUDIO },     // MPEG-3 Audio File
	{ _T(".mpa"),   ED2KFT_AUDIO },     // MPEG Audio File
	{ _T(".mpc"),   ED2KFT_AUDIO },     // Musepack Compressed Audio File
	{ _T(".mtm"),   ED2KFT_AUDIO },     // MultiTracker Module
	{ _T(".ogg"),   ED2KFT_AUDIO },     // Ogg Vorbis Compressed Audio File
	{ _T(".psm"),   ED2KFT_AUDIO },     // Protracker Studio Module
	{ _T(".ptm"),   ED2KFT_AUDIO },     // PolyTracker Module
	{ _T(".ra"),    ED2KFT_AUDIO },     // Real Audio File
	{ _T(".rmi"),   ED2KFT_AUDIO },     // MIDI File
	{ _T(".s3m"),   ED2KFT_AUDIO },     // Scream Tracker 3 Module
	{ _T(".snd"),   ED2KFT_AUDIO },     // Audio File (Sun, Unix)
	{ _T(".stm"),   ED2KFT_AUDIO },     // Scream Tracker 2 Module
	{ _T(".umx"),   ED2KFT_AUDIO },     // Unreal Music Package
	{ _T(".wav"),   ED2KFT_AUDIO },     // WAVE Audio File
	{ _T(".wma"),   ED2KFT_AUDIO },     // Windows Media Audio File
	{ _T(".xm"),    ED2KFT_AUDIO },     // Fasttracker 2 Extended Module

	{ _T(".3g2"),   ED2KFT_VIDEO },     // 3GPP Multimedia File
	{ _T(".3gp"),   ED2KFT_VIDEO },     // 3GPP Multimedia File
	{ _T(".3gp2"),  ED2KFT_VIDEO },     // 3GPP Multimedia File
	{ _T(".3gpp"),  ED2KFT_VIDEO },     // 3GPP Multimedia File
	{ _T(".amv"),   ED2KFT_VIDEO },     // Anime Music Video File
	{ _T(".asf"),   ED2KFT_VIDEO },     // Advanced Systems Format File
	{ _T(".avi"),   ED2KFT_VIDEO },     // Audio Video Interleave File
	{ _T(".bik"),   ED2KFT_VIDEO },     // BINK Video File
	{ _T(".divx"),  ED2KFT_VIDEO },     // DivX-Encoded Movie File
	{ _T(".dvr-ms"),ED2KFT_VIDEO },     // Microsoft Digital Video Recording
	{ _T(".flc"),   ED2KFT_VIDEO },     // FLIC Video File
	{ _T(".fli"),   ED2KFT_VIDEO },     // FLIC Video File
	{ _T(".flic"),  ED2KFT_VIDEO },     // FLIC Video File
	{ _T(".flv"),   ED2KFT_VIDEO },     // Flash Video File
	{ _T(".hdmov"), ED2KFT_VIDEO },     // High-Definition QuickTime Movie
	{ _T(".ifo"),   ED2KFT_VIDEO },     // DVD-Video Disc Information File
	{ _T(".m1v"),   ED2KFT_VIDEO },     // MPEG-1 Video File
	{ _T(".m2t"),   ED2KFT_VIDEO },     // MPEG-2 Video Transport Stream
	{ _T(".m2ts"),  ED2KFT_VIDEO },     // MPEG-2 Video Transport Stream
	{ _T(".m2v"),   ED2KFT_VIDEO },     // MPEG-2 Video File
	{ _T(".m4b"),   ED2KFT_VIDEO },     // MPEG-4 Video File
	{ _T(".m4v"),   ED2KFT_VIDEO },     // MPEG-4 Video File
	{ _T(".mkv"),   ED2KFT_VIDEO },     // Matroska Video File
	{ _T(".mov"),   ED2KFT_VIDEO },     // QuickTime Movie File
	{ _T(".movie"), ED2KFT_VIDEO },     // QuickTime Movie File
	{ _T(".mp1v"),  ED2KFT_VIDEO },     // MPEG-1 Video File        
	{ _T(".mp2v"),  ED2KFT_VIDEO },     // MPEG-2 Video File
	{ _T(".mp4"),   ED2KFT_VIDEO },     // MPEG-4 Video File
	{ _T(".mpe"),   ED2KFT_VIDEO },     // MPEG Video File
	{ _T(".mpeg"),  ED2KFT_VIDEO },     // MPEG Video File
	{ _T(".mpg"),   ED2KFT_VIDEO },     // MPEG Video File
	{ _T(".mpv"),   ED2KFT_VIDEO },     // MPEG Video File
	{ _T(".mpv1"),  ED2KFT_VIDEO },     // MPEG-1 Video File
	{ _T(".mpv2"),  ED2KFT_VIDEO },     // MPEG-2 Video File
	{ _T(".ogm"),   ED2KFT_VIDEO },     // Ogg Media File
	{ _T(".pva"),   ED2KFT_VIDEO },     // MPEG Video File
	{ _T(".qt"),    ED2KFT_VIDEO },     // QuickTime Movie
	{ _T(".ram"),   ED2KFT_VIDEO },     // Real Audio Media
	{ _T(".ratdvd"),ED2KFT_VIDEO },     // RatDVD Disk Image
	{ _T(".rm"),    ED2KFT_VIDEO },     // Real Media File
	{ _T(".rmm"),   ED2KFT_VIDEO },     // Real Media File
	{ _T(".rmvb"),  ED2KFT_VIDEO },     // Real Video Variable Bit Rate File
	{ _T(".rv"),    ED2KFT_VIDEO },     // Real Video File
	{ _T(".smil"),  ED2KFT_VIDEO },     // SMIL Presentation File
	{ _T(".smk"),   ED2KFT_VIDEO },     // Smacker Compressed Movie File
	{ _T(".swf"),   ED2KFT_VIDEO },     // Macromedia Flash Movie
	{ _T(".tp"),    ED2KFT_VIDEO },     // Video Transport Stream File
	{ _T(".ts"),    ED2KFT_VIDEO },     // Video Transport Stream File
	{ _T(".vid"),   ED2KFT_VIDEO },     // General Video File
	{ _T(".video"), ED2KFT_VIDEO },     // General Video File
	{ _T(".vob"),   ED2KFT_VIDEO },     // DVD Video Object File
	{ _T(".vp6"),   ED2KFT_VIDEO },     // TrueMotion VP6 Video File
	{ _T(".wm"),    ED2KFT_VIDEO },     // Windows Media Video File
	{ _T(".wmv"),   ED2KFT_VIDEO },     // Windows Media Video File
	{ _T(".xvid"),  ED2KFT_VIDEO },     // Xvid-Encoded Video File

	{ _T(".bmp"),   ED2KFT_IMAGE },     // Bitmap Image File
	{ _T(".emf"),   ED2KFT_IMAGE },     // Enhanced Windows Metafile
	{ _T(".gif"),   ED2KFT_IMAGE },     // Graphical Interchange Format File
	{ _T(".ico"),   ED2KFT_IMAGE },     // Icon File
	{ _T(".jfif"),  ED2KFT_IMAGE },     // JPEG File Interchange Format
	{ _T(".jpe"),   ED2KFT_IMAGE },     // JPEG Image File
	{ _T(".jpeg"),  ED2KFT_IMAGE },     // JPEG Image File
	{ _T(".jpg"),   ED2KFT_IMAGE },     // JPEG Image File
	{ _T(".pct"),   ED2KFT_IMAGE },     // PICT Picture File
	{ _T(".pcx"),   ED2KFT_IMAGE },     // Paintbrush Bitmap Image File
	{ _T(".pic"),   ED2KFT_IMAGE },     // PICT Picture File
	{ _T(".pict"),  ED2KFT_IMAGE },     // PICT Picture File
	{ _T(".png"),   ED2KFT_IMAGE },     // Portable Network Graphic
	{ _T(".psd"),   ED2KFT_IMAGE },     // Photoshop Document
	{ _T(".psp"),   ED2KFT_IMAGE },     // Paint Shop Pro Image File
	{ _T(".tga"),   ED2KFT_IMAGE },     // Targa Graphic
	{ _T(".tif"),   ED2KFT_IMAGE },     // Tagged Image File
	{ _T(".tiff"),  ED2KFT_IMAGE },     // Tagged Image File
	{ _T(".wmf"),   ED2KFT_IMAGE },     // Windows Metafile
	{ _T(".wmp"),   ED2KFT_IMAGE },     // Windows Media Photo File
	{ _T(".xif"),   ED2KFT_IMAGE },     // ScanSoft Pagis Extended Image Format File

	{ _T(".7z"),    ED2KFT_ARCHIVE },   // 7-Zip Compressed File
	{ _T(".ace"),   ED2KFT_ARCHIVE },   // WinAce Compressed File
	{ _T(".alz"),   ED2KFT_ARCHIVE },   // ALZip Archive
	{ _T(".arc"),   ED2KFT_ARCHIVE },   // Compressed File Archive
	{ _T(".arj"),   ED2KFT_ARCHIVE },   // ARJ Compressed File Archive
	{ _T(".bz2"),   ED2KFT_ARCHIVE },   // Bzip Compressed File
	{ _T(".cab"),   ED2KFT_ARCHIVE },   // Cabinet File
	{ _T(".cbr"),   ED2KFT_ARCHIVE },   // Comic Book RAR Archive
	{ _T(".cbz"),   ED2KFT_ARCHIVE },   // Comic Book ZIP Archive
	{ _T(".gz"),    ED2KFT_ARCHIVE },   // Gnu Zipped File
	{ _T(".hqx"),   ED2KFT_ARCHIVE },	// BinHex 4.0 Encoded File
	{ _T(".lha"),   ED2KFT_ARCHIVE },   // LHARC Compressed Archive
	{ _T(".lzh"),   ED2KFT_ARCHIVE },   // LZH Compressed File
	{ _T(".msi"),   ED2KFT_ARCHIVE },   // Microsoft Installer File
	{ _T(".pak"),   ED2KFT_ARCHIVE },   // PAK (Packed) File
	{ _T(".par"),   ED2KFT_ARCHIVE },   // Parchive Index File
	{ _T(".par2"),  ED2KFT_ARCHIVE },   // Parchive 2 Index File
	{ _T(".rar"),   ED2KFT_ARCHIVE },   // WinRAR Compressed Archive
	{ _T(".sit"),   ED2KFT_ARCHIVE },   // Stuffit Archive
	{ _T(".sitx"),  ED2KFT_ARCHIVE },   // Stuffit X Archive
	{ _T(".tar"),   ED2KFT_ARCHIVE },   // Consolidated Unix File Archive
	{ _T(".tbz2"),  ED2KFT_ARCHIVE },   // Tar BZip 2 Compressed File
	{ _T(".tgz"),   ED2KFT_ARCHIVE },   // Gzipped Tar File
	{ _T(".xpi"),   ED2KFT_ARCHIVE },   // Mozilla Installer Package
	{ _T(".z"),     ED2KFT_ARCHIVE },   // Unix Compressed File
	{ _T(".zip"),   ED2KFT_ARCHIVE },   // Zipped File

	{ _T(".bat"),   ED2KFT_PROGRAM },	// Batch File
	{ _T(".cmd"),   ED2KFT_PROGRAM },	// Command File
	{ _T(".com"),   ED2KFT_PROGRAM },	// COM File
	{ _T(".exe"),   ED2KFT_PROGRAM },	// Executable File
	{ _T(".hta"),   ED2KFT_PROGRAM },	// HTML Application
	{ _T(".js"),    ED2KFT_PROGRAM },	// Java Script
	{ _T(".jse"),   ED2KFT_PROGRAM },	// Encoded  Java Script
	{ _T(".msc"),   ED2KFT_PROGRAM },	// Microsoft Common Console File
	{ _T(".vbe"),   ED2KFT_PROGRAM },	// Encoded Visual Basic Script File
	{ _T(".vbs"),   ED2KFT_PROGRAM },	// Visual Basic Script File
	{ _T(".wsf"),   ED2KFT_PROGRAM },	// Windows Script File
	{ _T(".wsh"),   ED2KFT_PROGRAM },	// Windows Scripting Host File

	{ _T(".bin"),   ED2KFT_CDIMAGE },   // CD Image
	{ _T(".bwa"),   ED2KFT_CDIMAGE },   // BlindWrite Disk Information File
	{ _T(".bwi"),   ED2KFT_CDIMAGE },   // BlindWrite CD/DVD Disc Image
	{ _T(".bws"),   ED2KFT_CDIMAGE },   // BlindWrite Sub Code File
	{ _T(".bwt"),   ED2KFT_CDIMAGE },   // BlindWrite 4 Disk Image
	{ _T(".ccd"),   ED2KFT_CDIMAGE },   // CloneCD Disk Image
	{ _T(".cue"),   ED2KFT_CDIMAGE },   // Cue Sheet File
	{ _T(".dmg"),   ED2KFT_CDIMAGE },   // Mac OS X Disk Image
	{ _T(".img"),   ED2KFT_CDIMAGE },   // Disk Image Data File
	{ _T(".iso"),   ED2KFT_CDIMAGE },   // Disc Image File
	{ _T(".mdf"),   ED2KFT_CDIMAGE },   // Media Disc Image File
	{ _T(".mds"),   ED2KFT_CDIMAGE },   // Media Descriptor File
	{ _T(".nrg"),   ED2KFT_CDIMAGE },   // Nero CD/DVD Image File
	{ _T(".sub"),   ED2KFT_CDIMAGE },   // Subtitle File
	{ _T(".toast"), ED2KFT_CDIMAGE },   // Toast Disc Image

	{ _T(".chm"),   ED2KFT_DOCUMENT },  // Compiled HTML Help File
	{ _T(".css"),   ED2KFT_DOCUMENT },  // Cascading Style Sheet
	{ _T(".diz"),   ED2KFT_DOCUMENT },  // Description in Zip File
	{ _T(".doc"),   ED2KFT_DOCUMENT },  // Document File
	{ _T(".dot"),   ED2KFT_DOCUMENT },  // Document Template File
	{ _T(".hlp"),   ED2KFT_DOCUMENT },  // Help File
	{ _T(".htm"),   ED2KFT_DOCUMENT },  // HTML File
	{ _T(".html"),  ED2KFT_DOCUMENT },  // HTML File
	{ _T(".nfo"),   ED2KFT_DOCUMENT },  // Warez Information File
	{ _T(".pdf"),   ED2KFT_DOCUMENT },  // Portable Document Format File
	{ _T(".pps"),   ED2KFT_DOCUMENT },  // PowerPoint Slide Show
	{ _T(".ppt"),   ED2KFT_DOCUMENT },  // PowerPoint Presentation
	{ _T(".ps"),    ED2KFT_DOCUMENT },  // PostScript File
	{ _T(".rtf"),   ED2KFT_DOCUMENT },  // Rich Text Format File
	{ _T(".text"),  ED2KFT_DOCUMENT },  // General Text File
	{ _T(".txt"),   ED2KFT_DOCUMENT },  // Text File
	{ _T(".wri"),   ED2KFT_DOCUMENT },  // Windows Write Document
	{ _T(".xls"),   ED2KFT_DOCUMENT },  // Microsoft Excel Spreadsheet
	{ _T(".xml"),   ED2KFT_DOCUMENT },  // XML File

	{ _T(".emulecollection"), ED2KFT_EMULECOLLECTION }
};


CString ipstr(uint32 nIP)
{
	// following gives the same string as 'inet_ntoa(*(in_addr*)&nIP)' but is not restricted to ASCII strings
	const BYTE* pucIP = (BYTE*)&nIP;
	CString strIP;
	strIP.ReleaseBuffer(_stprintf(strIP.GetBuffer(3+1+3+1+3+1+3), _T("%u.%u.%u.%u"), pucIP[0], pucIP[1], pucIP[2], pucIP[3]));
	return strIP;
}

CString ipstr(uint32 nIP, uint16 nPort)
{
	// following gives the same string as 'inet_ntoa(*(in_addr*)&nIP)' but is not restricted to ASCII strings
	const BYTE* pucIP = (BYTE*)&nIP;
	CString strIP;
	strIP.ReleaseBuffer(_stprintf(strIP.GetBuffer(3+1+3+1+3+1+3+1+5), _T("%u.%u.%u.%u:%u"), pucIP[0], pucIP[1], pucIP[2], pucIP[3], nPort));
	return strIP;
}

CString ipstr(LPCTSTR pszAddress, uint16 nPort)
{
	CString strIPPort;
	strIPPort.Format(_T("%s:%u"), pszAddress, nPort);
	return strIPPort;
}

CStringA ipstrA(uint32 nIP)
{
	const BYTE* pucIP = (BYTE*)&nIP;
	CStringA strIP;
	strIP.ReleaseBuffer(sprintf(strIP.GetBuffer(3+1+3+1+3+1+3), "%u.%u.%u.%u", pucIP[0], pucIP[1], pucIP[2], pucIP[3]));
	return strIP;
}

void ipstrA(CHAR* pszAddress, int iMaxAddress, uint32 nIP)
{
	const BYTE* pucIP = (BYTE*)&nIP;
	_snprintf(pszAddress, iMaxAddress, "%u.%u.%u.%u", pucIP[0], pucIP[1], pucIP[2], pucIP[3]);
}

int utf8towc(LPCSTR pcUtf8, UINT uUtf8Size, LPWSTR pwc, UINT uWideCharSize)
{
	LPWSTR pwc0 = pwc;

	while (uUtf8Size && uWideCharSize)
	{
		BYTE ucChar = *pcUtf8++;
		if (ucChar < 0x80)
		{
			uUtf8Size--;
			uWideCharSize--;
			*(pwc++) = ucChar;
		}
		else if ((ucChar & 0xC0) != 0xC0)
		{
			return -1; // Invalid UTF8 string..
		}
		else
		{
			BYTE ucMask = 0xE0;
			UINT uExpectedBytes = 1;
			while ((ucChar & ucMask) == ucMask)
			{
				ucMask |= ucMask >> 1;
				if (++uExpectedBytes > 3)
					return -1; // Invalid UTF8 string..
			}

			if (uUtf8Size <= uExpectedBytes)
				return -1; // Invalid UTF8 string..

			UINT uProcessedBytes = 1 + uExpectedBytes;
			UINT uWideChar = (UINT)(ucChar & ~ucMask);
			if (uExpectedBytes == 1)
			{
				if ((uWideChar & 0x1E) == 0)
					return -1; // Invalid UTF8 string..
			}
			else
			{
				if (uWideChar == 0 && ((BYTE)*pcUtf8 & 0x3F & (ucMask << 1)) == 0)
					return -1; // Invalid UTF8 string..

				if (uExpectedBytes == 2)
				{
					//if (uWideChar == 0x0D && ((BYTE)*pcUtf8 & 0x20))
					//    return -1;
				}
				else if (uExpectedBytes == 3)
				{
					if (uWideChar > 4)
						return -1; // Invalid UTF8 string..
					if (uWideChar == 4 && ((BYTE)*pcUtf8 & 0x30))
						return -1; // Invalid UTF8 string..
				}
			}

			if (uWideCharSize < (UINT)(uExpectedBytes > 2) + 1)
				break; // buffer full

			while (uExpectedBytes--)
			{
				if (((ucChar = (BYTE)*(pcUtf8++)) & 0xC0) != 0x80)
					return -1; // Invalid UTF8 string..
				uWideChar <<= 6;
				uWideChar |= (ucChar & 0x3F);
			}
			uUtf8Size -= uProcessedBytes;

			if (uWideChar < 0x10000)
			{
				uWideCharSize--;
				*(pwc++) = (WCHAR)uWideChar;
			}
			else 
			{
				uWideCharSize -= 2;
				uWideChar -= 0x10000;
				*(pwc++) = (WCHAR)(0xD800 | (uWideChar >> 10));
				*(pwc++) = (WCHAR)(0xDC00 | (uWideChar & 0x03FF));
			}
		}
	}

	return pwc - pwc0;
}

int ByteStreamToWideChar(LPCSTR pcUtf8, UINT uUtf8Size, LPWSTR pwc, UINT uWideCharSize)
{
	int iWideChars = utf8towc(pcUtf8, uUtf8Size, pwc, uWideCharSize);
	if (iWideChars < 0)
	{
		LPWSTR pwc0 = pwc;
		while (uUtf8Size && uWideCharSize)
		{
			if ((*pwc++ = (BYTE)*pcUtf8++) == L'\0')
				break;
			uUtf8Size--;
			uWideCharSize--;
		}
		iWideChars = pwc - pwc0;
	}
	return iWideChars;
}

CStringA wc2utf8(const CStringW& rwstr)
{
	CStringA strUTF8;
	int iChars = AtlUnicodeToUTF8(rwstr, rwstr.GetLength(), NULL, 0);
	if (iChars > 0)
	{
		LPSTR pszUTF8 = strUTF8.GetBuffer(iChars);
		AtlUnicodeToUTF8(rwstr, rwstr.GetLength(), pszUTF8, iChars);
		strUTF8.ReleaseBuffer(iChars);
	}
	return strUTF8;
}

CString OptUtf8ToStr(const CStringA& rastr)
{
	CStringW wstr;
	int iMaxWideStrLen = rastr.GetLength();
	LPWSTR pwsz = wstr.GetBuffer(iMaxWideStrLen);
	int iWideChars = utf8towc(rastr, rastr.GetLength(), pwsz, iMaxWideStrLen);
	if (iWideChars <= 0)
	{
		// invalid UTF8 string...
		wstr.ReleaseBuffer(0);
		wstr = rastr;				// convert with local codepage
	}
	else
		wstr.ReleaseBuffer(iWideChars);
	return wstr;					// just return the string
}

CString OptUtf8ToStr(LPCSTR psz, int iLen)
{
	CStringW wstr;
	int iMaxWideStrLen = iLen;
	LPWSTR pwsz = wstr.GetBuffer(iMaxWideStrLen);
	int iWideChars = utf8towc(psz, iLen, pwsz, iMaxWideStrLen);
	if (iWideChars <= 0)
	{
		// invalid UTF8 string...
		wstr.ReleaseBuffer(0);
		wstr = psz;				// convert with local codepage
	}
	else
		wstr.ReleaseBuffer(iWideChars);
	return wstr;					// just return the string
}

CString OptUtf8ToStr(const CStringW& rwstr)
{
	CStringA astr;
	for (int i = 0; i < rwstr.GetLength(); i++)
	{
		if (rwstr[i] >= 0x100)
		{
			// this is no UTF8 string (it's already an Unicode string)...
			return rwstr;			// just return the string
		}
		astr += (BYTE)rwstr[i];
	}
	return OptUtf8ToStr(astr);
}

CStringA StrToUtf8(const CString& rstr)
{
	return wc2utf8(rstr);
}

bool IsValidEd2kString(LPCTSTR psz)
{
	while (*psz != _T('\0'))
	{
		// NOTE: The '?' is the character which is returned by windows if user entered an Unicode string into
		// an edit control (although application runs in ANSI mode).
		// The '?' is also invalid for search expressions and filenames.
		if (*psz == _T('?'))
			return false;
		psz++;
	}
	return true;
}

bool IsValidEd2kStringA(LPCSTR psz)
{
	while (*psz != '\0')
	{
		// NOTE: The '?' is the character which is returned by windows if user entered an Unicode string into
		// an edit control (although application runs in ANSI mode).
		// The '?' is also invalid for search expressions and filenames.
		if (*psz == '?')
			return false;
		psz++;
	}
	return true;
}

CString EncodeUrlUtf8(const CString& rstr)
{
	CString url;
	CStringA utf8(StrToUtf8(rstr));
	for (int i = 0; i < utf8.GetLength(); i++)
	{
		// NOTE: The purpose of that function is to encode non-ASCII characters only for being used within
		// an ED2K URL. An ED2K URL is not conforming to any RFC, thus any unsafe URI characters are kept
		// as they are. The space character is though special and gets encoded as well.
		if ((BYTE)utf8[i] == '%' || (BYTE)utf8[i] == ' ' || (BYTE)utf8[i] >= 0x7F)
			url.AppendFormat(_T("%%%02X"), (BYTE)utf8[i]);
		else
			url += utf8[i];
	}
	return url;
}

CStringW DecodeDoubleEncodedUtf8(LPCWSTR pszFileName)
{
	size_t nChars = wcslen(pszFileName);

	// Check if all characters are valid for UTF-8 value range
	//
	for (UINT i = 0; i < nChars; i++) {
		if ((_TUCHAR)pszFileName[i] > 0xFFU)
			return pszFileName; // string is already using Unicode character value range; return original
	}

	// Transform Unicode string to UTF-8 byte sequence
	//
	CStringA strA;
	LPSTR pszA = strA.GetBuffer(nChars);
	for (UINT i = 0; i < nChars; i++)
		pszA[i] = (CHAR)pszFileName[i];
	strA.ReleaseBuffer(nChars);

	// Decode the string with UTF-8
	//
	CStringW strW;
	LPWSTR pszW = strW.GetBuffer(nChars);
	int iNewChars = utf8towc(strA, nChars, pszW, nChars);
	if (iNewChars < 0) {
		strW.ReleaseBuffer(0);
		return pszFileName;		// conversion error (not a valid UTF-8 string); return original
	}
	strW.ReleaseBuffer(iNewChars);

	return strW;
}
bool _tmakepathlimit(TCHAR *path, const TCHAR *drive, const TCHAR *dir, const TCHAR *fname, const TCHAR *ext)
{
	if (path == NULL){
		ASSERT( false );
		return false;
	}

	uint32 nSize = 64; // the function should actually only add 4 (+1 nullbyte) bytes max extra
	if (drive != NULL)
		nSize += _tcsclen(drive);
	if (dir != NULL)
		nSize += _tcsclen(dir);
	if (fname != NULL)
		nSize += _tcsclen(fname);
	if (ext != NULL)
		nSize += _tcsclen(ext);

	TCHAR* tchBuffer = new TCHAR[nSize];
	_tmakepath(tchBuffer, drive, dir, fname, ext);

	if (_tcslen(tchBuffer) >= MAX_PATH){
		path[0] = _T('\0');
		ASSERT( false );
		delete[] tchBuffer;
		return false;
	}
	else{
		_tcscpy(path, tchBuffer);
		delete[] tchBuffer;
		return true;
	}
}
void DbgSetThreadName(LPCSTR szThreadName, ...) 
{
#ifdef DEBUG

#ifndef MS_VC_EXCEPTION
#define MS_VC_EXCEPTION 0x406d1388 

	typedef struct tagTHREADNAME_INFO 
	{
		DWORD dwType;		// must be 0x1000 
		LPCSTR szName;		// pointer to name (in same addr space) 
		DWORD dwThreadID;	// thread ID (-1 caller thread) 
		DWORD dwFlags;		// reserved for future use, must be zero 
	} THREADNAME_INFO; 
#endif

	__try
	{
		va_list args;
		va_start(args, szThreadName);
		int lenBuf = 0;
		char *buffer = NULL;
		int lenResult;
		do // the VS debugger truncates the string to 31 characters anyway!
		{
			lenBuf += 128;
			delete[] buffer;
			buffer = new char[lenBuf];
			lenResult = _vsnprintf(buffer, lenBuf, szThreadName, args);
		} while (lenResult == -1);
		va_end(args);
		THREADNAME_INFO info; 
		info.dwType = 0x1000; 
		info.szName = buffer; 
		info.dwThreadID = (DWORD)-1; 
		info.dwFlags = 0; 
		__try 
		{ 
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(DWORD), (DWORD *)&info); 
		} __except (EXCEPTION_CONTINUE_EXECUTION) { } 
		delete [] buffer;
	}
	__except (EXCEPTION_CONTINUE_EXECUTION) {}
#else
	UNREFERENCED_PARAMETER(szThreadName);
#endif
}
void Debug(LPCTSTR pszFmtMsg, ...)
{
	va_list pArgs;
	va_start(pArgs, pszFmtMsg);
	CString strBuff;
#ifdef _DEBUG
	time_t tNow = time(NULL);
	int iTimeLen = _tcsftime(strBuff.GetBuffer(40), 40, _T("%H:%M:%S "), localtime(&tNow));
	strBuff.ReleaseBuffer(iTimeLen);
#endif
	strBuff.AppendFormatV(pszFmtMsg, pArgs);

	// get around a bug in the debug device which is not capable of dumping long strings
	int i = 0;
	while (i < strBuff.GetLength()){
		OutputDebugString(strBuff.Mid(i, 1024));
		i += 1024;
	}
	va_end(pArgs);
}
void DebugSend(LPCSTR pszOpcode, uint32 ip, uint16 port)
{
	TCHAR szIPPort[22];
	_stprintf(szIPPort, _T("%s:%u"), ipstr(ntohl(ip)), port);
	Debug(_T(">>> %-20hs to   %-21s\n"), pszOpcode, szIPPort);
}

void DebugSendF(LPCSTR pszOpcode, uint32 ip, uint16 port, LPCTSTR pszMsg, ...)
{
	va_list args;
	va_start(args, pszMsg);
	TCHAR szIPPort[22];
	_stprintf(szIPPort, _T("%s:%u"), ipstr(ntohl(ip)), port);
	CString str;
	str.Format(_T(">>> %-20hs to   %-21s; "), pszOpcode, szIPPort);
	str.AppendFormatV(pszMsg, args);
	va_end(args);
	Debug(_T("%s\n"), str);
}

void DebugRecv(LPCSTR pszOpcode, uint32 ip, uint16 port)
{
	TCHAR szIPPort[22];
	_stprintf(szIPPort, _T("%s:%u"), ipstr(ntohl(ip)), port);
	Debug(_T("%-24hs from %-21s\n"), pszOpcode, szIPPort);
}

uint16 GetRandomUInt16()
{
#if RAND_MAX == 0x7fff
	// get 2 random numbers
	UINT uRand0 = rand();
	UINT uRand1 = rand();

	// NOTE: if that assert fires, you have most likely called that function *without* calling 'srand' first.
	// NOTE: each spawned thread HAS to call 'srand' for itself to get real random numbers.
	ASSERT( !(uRand0 == 41 && uRand1 == 18467) );

	return (uint16)(uRand0 | ((uRand1 >= RAND_MAX/2) ? 0x8000 : 0x0000));
#else
#error "Implement it!"
#endif
}

uint32 GetRandomUInt32()
{
#if RAND_MAX == 0x7fff
	//return ((uint32)GetRandomUInt16() << 16) | (uint32)GetRandomUInt16();
	// this would give the receiver the following information:
	//	random number N
	//	random number N+1 is below or greater/equal than 0x8000
	//	random number N+2
	//	random number N+3 is below or greater/equal than 0x8000

	uint32 uRand0 = GetRandomUInt16();
	srand(GetTickCount() | uRand0);
	uint32 uRand1 = GetRandomUInt16();
	return (uRand0 << 16) | uRand1;
#else
#error "Implement it!"
#endif
}
bool IsLANIP(uint32 nIP)
{
	// LAN IP's
	// -------------------------------------------
	//	0.*								"This" Network
	//	10.0.0.0 - 10.255.255.255		Class A
	//	172.16.0.0 - 172.31.255.255		Class B
	//	192.168.0.0 - 192.168.255.255	Class C

	uint8 nFirst = (uint8)nIP;
	uint8 nSecond = (uint8)(nIP >> 8);

	if (nFirst==192 && nSecond==168) // check this 1st, because those LANs IPs are mostly spreaded
		return true;

	if (nFirst==172 && nSecond>=16 && nSecond<=31)
		return true;

	if (nFirst==0 || nFirst==10)
		return true;

	return false; 
}
bool IsGoodIP(uint32 nIP, bool forceCheck)
{
	// always filter following IP's
	// -------------------------------------------
	// 0.0.0.0							invalid
	// 127.0.0.0 - 127.255.255.255		Loopback
	// 224.0.0.0 - 239.255.255.255		Multicast
	// 240.0.0.0 - 255.255.255.255		Reserved for Future Use
	// 255.255.255.255					invalid

	// 	if (nIP==0 || (uint8)nIP==127 || (uint8)nIP>=224){
	// #ifdef _DEBUG
	// 		if (nIP==0x0100007F && thePrefs.GetAllowLocalHostIP())
	// 			return true;
	// #endif
	// 		return false;
	// 	}
	// 
	// 	if (!thePrefs.FilterLANIPs() && !forceCheck/*ZZ:UploadSpeedSense*/)
	// 		return true;
	// 	else
	return !IsLANIP(nIP);
}
bool IsGoodIPPort(uint32 nIP, uint16 nPort)
{
	return IsGoodIP(nIP) && nPort!=0;
}

int GetPathDriveNumber(CString path)
{
	if (path.GetLength()<3 || path.GetAt(1)!=_T(':') || path.GetAt(2)!=_T('\\'))
		return -1;

	return path.MakeLower().GetAt(0) - 97;
}



bool gotostring(CFile &file, const uchar *find, LONGLONG plen)
{
	bool found = false;
	LONGLONG i=0;
	LONGLONG j=0;
	//LONGLONG plen = strlen(find);
	LONGLONG len = file.GetLength() - file.GetPosition();
	uchar temp;

	while (!found && i < len)
	{
		file.Read(&temp,1);
		if (temp == find[j])
			j++;
		else if(temp == find[0])
			j=1;
		else
			j=0;
		if (j==plen)
			return true;
		i++;
	}
	return false;
}

static void swap_byte (uint8* a, uint8* b){
	uint8 bySwap;
	bySwap = *a;
	*a = *b;
	*b = bySwap;
}


RC4_Key_Struct* RC4CreateKey(const uchar* pachKeyData, uint32 nLen, RC4_Key_Struct* key, bool bSkipDiscard){
	uint8 index1;
	uint8 index2;
	uint8* pabyState;

	if (key == NULL)
		key = new RC4_Key_Struct;

	pabyState= &key->abyState[0];
	for (int i = 0; i < 256; i++)
		pabyState[i] = (uint8)i;

	key->byX = 0;
	key->byY = 0;
	index1 = 0;
	index2 = 0;
	for (int i = 0; i < 256; i++){
		index2 = (pachKeyData[index1] + pabyState[i] + index2);
		swap_byte(&pabyState[i], &pabyState[index2]);
		index1 = (uint8)((index1 + 1) % nLen);
	}
	if (!bSkipDiscard)
		RC4Crypt(NULL, NULL, 1024, key);
	return key;
}

void RC4Crypt(const uchar* pachIn, uchar* pachOut, uint32 nLen, RC4_Key_Struct* key){
	ASSERT( key != NULL && nLen > 0 );
	if (key == NULL)
		return;

	uint8 byX = key->byX;;
	uint8 byY = key->byY;
	uint8* pabyState = &key->abyState[0];;
	uint8 byXorIndex;

	for (uint32 i = 0; i < nLen; i++)
	{
		byX = (byX + 1);
		byY = (pabyState[byX] + byY);
		swap_byte(&pabyState[byX], &pabyState[byY]);
		byXorIndex = (pabyState[byX] + pabyState[byY]);

		if (pachIn != NULL)
			pachOut[i] = pachIn[i] ^ pabyState[byXorIndex];
	}
	key->byX = byX;
	key->byY = byY;
}

int CompareLocaleStringNoCase(LPCTSTR psz1, LPCTSTR psz2)
{
	// SDK says: The 'CompareString' function is optimized to run at the highest speed when 'dwCmpFlags' is set to 0 
	// or NORM_IGNORECASE, and 'cchCount1' and 'cchCount2' have the value -1.
	int iResult = CompareString(GetThreadLocale(), NORM_IGNORECASE, psz1, -1, psz2, -1);
	if (iResult == 0)
		return 0;
	return iResult - 2;
}

bool strmd4(const char* pszHash, uchar* hash)
{
	memset(hash, 0, 16);
	for (int i = 0; i < 16; i++)
	{
		char byte[3];
		byte[0] = pszHash[i*2+0];
		byte[1] = pszHash[i*2+1];
		byte[2] = '\0';

		UINT b;
		if (sscanf(byte, "%x", &b) != 1)
			return false;
		hash[i] = (char)b;
	}
	return true;
}
bool strmd4(const CString& rstr, uchar* hash)
{
	memset(hash, 0, 16);
	if (rstr.GetLength() != 16*2)
		return false;
	for (int i = 0; i < 16; i++)
	{
		char byte[3];
		byte[0] = (char)rstr[i*2+0];
		byte[1] = (char)rstr[i*2+1];
		byte[2] = '\0';

		UINT b;
		if (sscanf(byte, "%x", &b) != 1)
			return false;
		hash[i] = (char)b;
	}
	return true;
}
// Returns a BASE32 encoded byte array
//
// [In]
//   buffer: Pointer to byte array
//   bufLen: Lenght of buffer array
//
// [Return]
//   CString object with BASE32 encoded byte array
CString EncodeBase32(const unsigned char* buffer, unsigned int bufLen)
{
	CString Base32Buff;

	unsigned int i, index;
	unsigned char word;

	for(i = 0, index = 0; i < bufLen;) {

		// Is the current word going to span a byte boundary?
		if (index > 3) {
			word = (BYTE)(buffer[i] & (0xFF >> index));
			index = (index + 5) % 8;
			word <<= index;
			if (i < bufLen - 1)
				word |= buffer[i + 1] >> (8 - index);

			i++;
		} else {
			word = (BYTE)((buffer[i] >> (8 - (index + 5))) & 0x1F);
			index = (index + 5) % 8;
			if (index == 0)
				i++;
		}

		Base32Buff += (char) base32Chars[word];
	}

	return Base32Buff;
}

// Returns a BASE16 encoded byte array
//
// [In]
//   buffer: Pointer to byte array
//   bufLen: Lenght of buffer array
//
// [Return]
//   CString object with BASE16 encoded byte array
CString EncodeBase16(const unsigned char* buffer, unsigned int bufLen)
{
	CString Base16Buff;

	for(unsigned int i = 0; i < bufLen; i++) {
		Base16Buff += base16Chars[buffer[i] >> 4];
		Base16Buff += base16Chars[buffer[i] & 0xf];
	}

	return Base16Buff;
}

// Decodes a BASE16 string into a byte array
//
// [In]
//   base16Buffer: String containing BASE16
//   base16BufLen: Lenght BASE16 coded string's length
//
// [Out]
//   buffer: byte array containing decoded string
bool DecodeBase16(const TCHAR *base16Buffer, unsigned int base16BufLen, byte *buffer, unsigned int bufflen)
{
	unsigned int uDecodeLengthBase16 = DecodeLengthBase16(base16BufLen);
	if (uDecodeLengthBase16 > bufflen)
		return false;
	memset(buffer, 0, uDecodeLengthBase16);

	for(unsigned int i = 0; i < base16BufLen; i++) {
		int lookup = _totupper((_TUCHAR)base16Buffer[i]) - _T('0');

		// Check to make sure that the given word falls inside a valid range
		byte word = 0;

		if ( lookup < 0 || lookup >= BASE16_LOOKUP_MAX)
			word = 0xFF;
		else
			word = base16Lookup[lookup][1];

		if(i % 2 == 0) {
			buffer[i/2] = word << 4;
		} else {
			buffer[(i-1)/2] |= word;
		}
	}
	return true;
}

// Calculates length to decode from BASE16
//
// [In]
//   base16Length: Actual length of BASE16 string
//
// [Return]
//   New length of byte array decoded
unsigned int DecodeLengthBase16(unsigned int base16Length)
{
	return base16Length / 2U;
}

uint32 DecodeBase32(LPCTSTR pszInput, uchar* paucOutput, uint32 nBufferLen)
{
	if (pszInput == NULL)
		return false;
	uint32 nDecodeLen = (_tcslen(pszInput)*5)/8;
	if ((_tcslen(pszInput)*5) % 8 > 0)
		nDecodeLen++;
	uint32 nInputLen = _tcslen( pszInput );
	if (paucOutput == NULL || nBufferLen == 0)
		return nDecodeLen;
	if (nDecodeLen > nBufferLen || paucOutput == NULL) 
		return 0;

	DWORD nBits	= 0;
	int nCount	= 0;

	for ( int nChars = nInputLen ; nChars-- ; pszInput++ )
	{
		if ( *pszInput >= 'A' && *pszInput <= 'Z' )
			nBits |= ( *pszInput - 'A' );
		else if ( *pszInput >= 'a' && *pszInput <= 'z' )
			nBits |= ( *pszInput - 'a' );
		else if ( *pszInput >= '2' && *pszInput <= '7' )
			nBits |= ( *pszInput - '2' + 26 );
		else
			return 0;

		nCount += 5;

		if ( nCount >= 8 )
		{
			*paucOutput++ = (BYTE)( nBits >> ( nCount - 8 ) );
			nCount -= 8;
		}

		nBits <<= 5;
	}

	return nDecodeLen;
}

uint32 DecodeBase32(LPCTSTR pszInput, CAICHHash& Hash)
{
	return DecodeBase32(pszInput, Hash.GetRawHash(), Hash.GetHashSize());
}
int __cdecl CompareE2DKFileType(const void* p1, const void* p2)
{
	return _tcscmp( ((const SED2KFileType*)p1)->pszExt, ((const SED2KFileType*)p2)->pszExt );
}

EED2KFileType GetED2KFileTypeID(LPCTSTR pszFileName)
{
	LPCTSTR pszExt = _tcsrchr(pszFileName, _T('.'));
	if (pszExt == NULL)
		return ED2KFT_ANY;
	CString strExt(pszExt);
	strExt.MakeLower();

	SED2KFileType ft;
	ft.pszExt = strExt;
	ft.iFileType = ED2KFT_ANY;
	const SED2KFileType* pFound = (SED2KFileType*)bsearch(&ft, g_aED2KFileTypes, _countof(g_aED2KFileTypes), sizeof g_aED2KFileTypes[0], CompareE2DKFileType);
	if (pFound != NULL)
		return pFound->iFileType;
	return ED2KFT_ANY;
}

// Retuns the ed2k file type string ID which is to be used for publishing+searching
LPCSTR GetED2KFileTypeSearchTerm(EED2KFileType iFileID)
{
	if (iFileID == ED2KFT_AUDIO)			return ED2KFTSTR_AUDIO;
	if (iFileID == ED2KFT_VIDEO)			return ED2KFTSTR_VIDEO;
	if (iFileID == ED2KFT_IMAGE)			return ED2KFTSTR_IMAGE;
	if (iFileID == ED2KFT_PROGRAM)			return ED2KFTSTR_PROGRAM;
	if (iFileID == ED2KFT_DOCUMENT)			return ED2KFTSTR_DOCUMENT;
	// NOTE: Archives and CD-Images are published+searched with file type "Pro"
	// NOTE: If this gets changed, the function 'GetED2KFileTypeSearchID' also needs to get updated!
	if (iFileID == ED2KFT_ARCHIVE)			return ED2KFTSTR_PROGRAM;
	if (iFileID == ED2KFT_CDIMAGE)			return ED2KFTSTR_PROGRAM;
	if (iFileID == ED2KFT_EMULECOLLECTION)	return ED2KFTSTR_EMULECOLLECTION;
	return NULL;
}

// Retuns the ed2k file type integer ID which is to be used for publishing+searching
EED2KFileType GetED2KFileTypeSearchID(EED2KFileType iFileID)
{
	if (iFileID == ED2KFT_AUDIO)			return ED2KFT_AUDIO;
	if (iFileID == ED2KFT_VIDEO)			return ED2KFT_VIDEO;
	if (iFileID == ED2KFT_IMAGE)			return ED2KFT_IMAGE;
	if (iFileID == ED2KFT_PROGRAM)			return ED2KFT_PROGRAM;
	if (iFileID == ED2KFT_DOCUMENT)			return ED2KFT_DOCUMENT;
	// NOTE: Archives and CD-Images are published+searched with file type "Pro"
	// NOTE: If this gets changed, the function 'GetED2KFileTypeSearchTerm' also needs to get updated!
	if (iFileID == ED2KFT_ARCHIVE)			return ED2KFT_PROGRAM;
	if (iFileID == ED2KFT_CDIMAGE)			return ED2KFT_PROGRAM;
	return ED2KFT_ANY;
}

// Returns a file type which is used eMule internally only, examining the extension of the given filename
CString GetFileTypeByName(LPCTSTR pszFileName)
{
	EED2KFileType iFileType = GetED2KFileTypeID(pszFileName);
	switch (iFileType) {
	case ED2KFT_AUDIO:			return _T(ED2KFTSTR_AUDIO);
	case ED2KFT_VIDEO:			return _T(ED2KFTSTR_VIDEO);
	case ED2KFT_IMAGE:			return _T(ED2KFTSTR_IMAGE);
	case ED2KFT_DOCUMENT:		return _T(ED2KFTSTR_DOCUMENT);
	case ED2KFT_PROGRAM:		return _T(ED2KFTSTR_PROGRAM);
	case ED2KFT_ARCHIVE:		return _T(ED2KFTSTR_ARCHIVE);
	case ED2KFT_CDIMAGE:		return _T(ED2KFTSTR_CDIMAGE);
	case ED2KFT_EMULECOLLECTION:return _T(ED2KFTSTR_EMULECOLLECTION);
	default:					return _T("");
	}
}
void md4str(const uchar* hash, TCHAR* pszHash)
{
	static const TCHAR _acHexDigits[] = _T("0123456789ABCDEF");
	for (int i = 0; i < 16; i++){
		*pszHash++ = _acHexDigits[hash[i] >> 4];
		*pszHash++ = _acHexDigits[hash[i] & 0xf];
	}
	*pszHash = _T('\0');
}

CString md4str(const uchar* hash)
{
	TCHAR szHash[32+1];
	md4str(hash, szHash);
	CString ret(szHash);

	// 	OutputDebugString(ret);
	// 	OutputDebugString(_T("\r\n"));
	return ret;
}
uint32 LevenshteinDistance(const CString& str1, const CString& str2)
{
	uint32 n1 = str1.GetLength();
	uint32 n2 = str2.GetLength();

	uint32* p = new uint32[n2+1];
	uint32* q = new uint32[n2+1];
	uint32* r;

	p[0] = 0;
	for(uint32 j = 1; j <= n2; ++j)
		p[j] = p[j-1] + 1;

	for(uint32 i = 1; i <= n1; ++i)
	{
		q[0] = p[0] + 1;
		for(uint32 j = 1; j <= n2; ++j )
		{
			uint32 d_del = p[j] + 1;
			uint32 d_ins = q[j-1] + 1;
			uint32 d_sub = p[j-1] + (str1.GetAt(i-1) == str2.GetAt(j-1) ? 0 : 1);
			q[j] = min(min(d_del, d_ins), d_sub);
		}
		r = p;
		p = q;
		q = r;
	}

	uint32 tmp = p[n2];
	delete[] p;
	delete[] q;

	return tmp;
}
CString GetFormatedUInt64(ULONGLONG ullVal)
{
	TCHAR szVal[24];
	_ui64tot(ullVal, szVal, 10);

	static NUMBERFMT nf;
	if (nf.Grouping == 0) {
		nf.NumDigits = 0;
		nf.LeadingZero = 0;
		nf.Grouping = 3;
		// we are hardcoding the following two format chars by intention because the C-RTL also has the decimal sep hardcoded to '.'
		nf.lpDecimalSep = _T(".");
		nf.lpThousandSep = _T(",");
		nf.NegativeOrder = 0;
	}
	CString strVal;
	const int iBuffSize = _countof(szVal)*2;
	int iResult = GetNumberFormat(LOCALE_SYSTEM_DEFAULT, 0, szVal, &nf, strVal.GetBuffer(iBuffSize), iBuffSize);
	strVal.ReleaseBuffer();
	if (iResult == 0)
		strVal = szVal;
	return strVal;
}
CString StripInvalidFilenameChars(const CString& strText)
{
	LPCTSTR pszSource = strText;
	CString strDest;

	while (*pszSource != _T('\0'))
	{
		if (!(((_TUCHAR)*pszSource >= 0 && (_TUCHAR)*pszSource <= 31) ||
			// lots of invalid chars for filenames in windows :=)
			*pszSource == _T('\"') || *pszSource == _T('*') || *pszSource == _T('<')  || *pszSource == _T('>') ||
			*pszSource == _T('?')  || *pszSource == _T('|') || *pszSource == _T('\\') || *pszSource == _T('/') || 
			*pszSource == _T(':')) )
		{
			strDest += *pszSource;
		}
		pszSource++;
	}

	static const LPCTSTR apszReservedFilenames[] = {
		_T("NUL"), _T("CON"), _T("PRN"), _T("AUX"), _T("CLOCK$"),
		_T("COM1"),_T("COM2"),_T("COM3"),_T("COM4"),_T("COM5"),_T("COM6"),_T("COM7"),_T("COM8"),_T("COM9"),
		_T("LPT1"),_T("LPT2"),_T("LPT3"),_T("LPT4"),_T("LPT5"),_T("LPT6"),_T("LPT7"),_T("LPT8"),_T("LPT9")
	};
	for (int i = 0; i < _countof(apszReservedFilenames); i++)
	{
		int nPrefixLen = _tcslen(apszReservedFilenames[i]);
		if (_tcsnicmp(strDest, apszReservedFilenames[i], nPrefixLen) == 0)
		{
			if (strDest.GetLength() == nPrefixLen) {
				// Filename is a reserved file name:
				// Append an underscore character
				strDest += _T("_");
				break;
			}
			else if (strDest[nPrefixLen] == _T('.')) {
				// Filename starts with a reserved file name followed by a '.' character:
				// Replace that ',' character with an '_' character.
				LPTSTR pszDest = strDest.GetBuffer(strDest.GetLength());
				pszDest[nPrefixLen] = _T('_');
				strDest.ReleaseBuffer(strDest.GetLength());
				break;
			}
		}
	}

	return strDest;
}

CString CastItoXBytes(uint16 count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}

CString CastItoXBytes(uint32 count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}

CString CastItoXBytes(uint64 count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}

#if defined(_DEBUG) && defined(USE_DEBUG_EMFILESIZE)
CString CastItoXBytes(EMFileSize count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}
#endif

CString CastItoXBytes(float count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}
CString CastItoXBytes(double count, bool isK, bool isPerSec, uint32 decimal)
{
	if( count <= 0.0 )
	{
		if(isPerSec)
		{
			return _T("0B/s");
		}
		else
			return _T("0字节") ;
	}
	else if( isK )
	{
		if( count >  1.7E+300 )
			count =  1.7E+300;
		else
			count *= 1024.0;
	}
	CString buffer;
	if( isPerSec )
	{
		if (false)
			buffer.Format(_T("%.*f %s"), decimal, count/1024.0, _T("KB/s"));
		else if (count < 1024.0)
			buffer.Format(_T("%.0f %s"), count,_T("B/s"));
		else if (count < 1024000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1024.0, _T("KB/s"));
		else if (count < 1048576000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1048576.0, _T("MB/s"));
		else if (count < 1073741824000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1073741824.0,_T("GB/秒"));
		else 
			buffer.Format(_T("%.*f %s"), decimal, count/1099511627776.0, _T("TB/秒"));
	}
	else
	{
		if (count < 1024000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1024.0, _T("KB"));
		else if (count < 1048576000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1048576.0, _T("MB"));
		else if (count < 1073741824000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1073741824.0, _T("GB"));
		else 
			buffer.Format(_T("%.*f %s"), decimal, count/1099511627776.0, _T("TB"));
	}
	return buffer;
}

BOOL GetMACAddress(TCHAR szMACAddress[LEN_NETWORK_ID])
{
	//变量定义
	HINSTANCE hInstance=NULL;

	//执行逻辑
	__try
	{
		//加载 DLL
		hInstance=LoadLibrary(TEXT("NetApi32.dll"));
		if (hInstance==NULL) __leave;

		//获取函数
		typedef BYTE __stdcall NetBiosProc(NCB * Ncb);
		NetBiosProc * pNetBiosProc=(NetBiosProc *)GetProcAddress(hInstance,"Netbios");
		if (pNetBiosProc==NULL) __leave;

		//变量定义
		NCB Ncb;
		LANA_ENUM LanaEnum;
		ZeroMemory(&Ncb,sizeof(Ncb));
		ZeroMemory(&LanaEnum,sizeof(LanaEnum));

		//枚举网卡
		Ncb.ncb_command=NCBENUM;
		Ncb.ncb_length=sizeof(LanaEnum);
		Ncb.ncb_buffer=(BYTE *)&LanaEnum;
		if ((pNetBiosProc(&Ncb)!=NRC_GOODRET)||(LanaEnum.length==0)) __leave;

		//获取地址
		if (LanaEnum.length>0)
		{
			//变量定义
			tagAstatInfo Adapter;
			ZeroMemory(&Adapter,sizeof(Adapter));

			//重置网卡
			Ncb.ncb_command=NCBRESET;
			Ncb.ncb_lana_num=LanaEnum.lana[0];
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) __leave;

			//获取状态
			Ncb.ncb_command=NCBASTAT;
			Ncb.ncb_length=sizeof(Adapter);
			Ncb.ncb_buffer=(BYTE *)&Adapter;
			Ncb.ncb_lana_num=LanaEnum.lana[0];
			strcpy((char *)Ncb.ncb_callname,"*");
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) __leave;

			//获取地址
			for (INT i=0;i<6;i++)
			{
				ASSERT((i*2)<LEN_NETWORK_ID);
				_stprintf(&szMACAddress[i*2],TEXT("%02X"),Adapter.AdapterStatus.adapter_address[i]);
			}
		}
	}

	//结束清理
	__finally
	{
		//释放资源
		if (hInstance!=NULL)
		{
			FreeLibrary(hInstance);
			hInstance=NULL;
		}

		//错误断言
		if (AbnormalTermination()==TRUE)
		{
			ASSERT(FALSE);
		}
	}

	return true;
}

CString GetWindowVersion() 
{ 
	OSVERSIONINFO osver; 
	int m_nOSVersion;
	osver.dwOSVersionInfoSize=sizeof(OSVERSIONINFO); 
	GetVersionEx(&osver); 
	CString strRet = _T("windowsxp");
	//if(osver.dwPlatformId== VER_PLATFORM_WIN32_WINDOWS) 
	//{  
		if (osver.dwPlatformId == VER_PLATFORM_WIN32_NT) 
		{ 
			if(osver.dwMajorVersion == 5.0) 
			{ 
				strRet = _T("Windows2000");
			} 
			else if (osver.dwMajorVersion == 5.1) 
			{
				strRet = _T("WindowsXP");
			}
			else if (osver.dwMajorVersion == 5.2) 
			{
				strRet = _T("WindowsServer2003");
			}
			else if (osver.dwMajorVersion == 6.0) 
			{
				strRet = _T("WindowsVista");
			}
			else if (osver.dwMajorVersion == 6.2) 
			{
				strRet = _T("Windows8");
			}
		}
	//} 
	return strRet;
} 
