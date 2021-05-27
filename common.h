#ifndef COMMON_H
#define COMMON_H

#include "windows.h"
#include "windowsx.h"

#include "../KadLibrary/Kademlia/kadlibrary.h"

typedef __int64 int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef unsigned __int64 uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#define __T(x) L ## x
#define _T(x) __T(x)

#define CONN(x,y) x##y
#define TOCHAR(x) #@x
#define TOSTRING(x) #x
#define STR_CONN(x,y) x y


#define SHADOW_WIDTH		5
#define BORDER_MAX_WIDTH	8
#define BORDER_MIN_WIDTH	2
#define MAIN_WINDOW_WIDTH	1024
#define MAIN_WINDOW_HEIGHT	768
#define SKIN_WINDOW_WIDTH	620
#define SKIN_WINDOW_HEIGHT	445
#define CHANGE_SKIN_WIDGET_WIDTH	140
#define CHANGE_SKIN_WIDGET_HEIGHT	160
#define CLOUD_WINDOW_WIDTH	1024
#define CLOUD_WINDOW_HEIGHT	768
#define MESSAGE_WINDOW_WIDTH	380
#define MESSAGE_WINDOW_HEIGHT	180
#define MINI_PORTAL_WINDOW_WIDTH	704
#define MINI_PORTAL_WINDOW_HEIGHT	450
#define PLAY_WINDOW_WIDTH	800
#define PLAY_WINDOW_HEIGHT	600
#define SETTING_WINDOW_WIDTH	700
#define SETTING_WINDOW_HEIGHT	450
#define FEEDBACK_WINDOW_WIDTH	700
#define FEEDBACK_WINDOW_HEIGHT	400
#define SKIN_COUNT 15

//#define KAD_SEARCHER
#define LOCAL_KAD_LIB

#ifdef KAD_SEARCHER
	#define APP_TITLE "KadSearcher"
	#define STARTUP_URL "http://kadsearcher.com:82/Default.aspx"
	#define WEB_SERVICE_URL "http://www.kadsearcher.com:81/kadservice.asmx"
	#define HELP_URL "http://www.kadsearcher.com/blogpost_help.html"
	#define WEBSITE_URL "http://www.kadsearcher.com"
	#define APP_VERSION 1,0,0,9
	#define APP_VERSION_STR "1.0.0.9"
#else
	#define APP_TITLE "KadPlayer"
	#define STARTUP_URL "http://kadplayer.com:82/Default.aspx"
	#define WEB_SERVICE_URL "http://www.kadplayer.com:81/kadservice.asmx"
	#define HELP_URL "http://www.kadplayer.com/blogpost_help.html"
	#define WEBSITE_URL "http://www.kadplayer.com"
	#define APP_VERSION 1,0,0,9
	#define APP_VERSION_STR "1.0.0.9"
#endif

//#define TODAY_RESOURCE
#define UPDATER_FILE_NAME           "update.exe"
#define MINI_PORTAL_URL "http://www.2345.com/?25908"
#define THUNDER_PLAY_URL "http://vod.xunlei.com/client/cplayer.html?uvs=%s_%s_%s&url=%s&from=vodClientHome"
#define THUNDER_INIT_URL "http://vod.xunlei.com/client/chome.html?clientAction=init"
#define THUNDER_LOAD_URL "http://vod.xunlei.com/client/chome.html?list=clist"
#define THUNDER_CLOUD_PLAY_URL	"http://vod.xunlei.com/client/chome.html?clientAction=init"

#define HUOYAN_PLAY_URL "http://www.huoyan.tv/api.php#!u="
#define HUOYAN_CLOUD_PLAY_URL	"http://api.huoyan.tv/"


#define DES_KEY "imkadkey"
#define SETTING_FILE_NAME "setting.ini"
#define IMAGE_DIR_NAME "Image"
#define DATE_FORMAT "yyyy-MM-dd"
#define TIME_FORMAT "HH:mm:ss"
#define DATE_TIME_FORMAT "yyyy-MM-dd HH:mm:ss"
#define TIMEOUT_SECONDS 60000

#define TEN_MILLI_SECONDS 10
#define FIFTY_MILLI_SECONDS 50
#define HUNDRED_MILLI_SECONDS 100
#define QUARTER_SECOND 250
#define HALF_SECOND 500
#define ONE_SECOND 1000
#define TWO_SECONDS 2000
#define FIVE_SECONDS 5000
#define TEN_SECONDS 10000
#define THIRTY_SECONDS 30000
#define ONE_MINUTE 60000
#define THREE_MINUTES 180000
#define HALF_HOUR 1800000
#define ONE_HOUR 3600000

enum UpdateMessageType
{
	UPDATE_EXIST = WM_USER + 100,
	UPDATE_NO_EXIST,
	UPDATE_DOWNLOAD_SUCCESS,
	UPDATE_DOWNLOAD_FAIL,
};

enum ConnectStatusType
{
	CONNECT_UNKNOW = 0,
	CONNECT_CONNECTING,
	CONNECT_ERROR,
	CONNECT_CONNECTED,
};

enum ResultTableViewColumnType
{
	COLUMN_ID = 0,
	COLUMN_NAME,
	COLUMN_SIZE,
	COLUMN_TYPE,
	COLUMN_RESOURCE,
	COLUMN_OPERATE,
};

enum PlayChannelType
{
	CHANNEL_THUNDER = 0,
	CHANNEL_QQ,
	CHANNEL_OTHER,
};

enum DouleClickType
{
	CLICK_PLAY = 0,
	CLICK_DOWNLOAD,
	CLICK_COPY,
};

enum FeedbackType
{
	FEEDBACK_BUG = 0,
	FEEDBACK_SEX,
	FEEDBACK_PIRATE,
};

#pragma pack (push)
#pragma pack(1)
typedef struct _SRESULT
{
	_SRESULT()
	{
		Size = 0;
		memset(Name, 0, sizeof(Name));
		memset(Hash, 0, sizeof(Hash));
		Resource = 0;
	}
	unsigned __int64 Size;
	char Name[0x400];
	BYTE Hash[0x10];
	int Resource;

}SRESULT, *PSRESULT;
#pragma pack (pop)

typedef int (WINAPI *Init)(int cmd);
typedef int (*Uninit)();
typedef int (*DeleteFileBasicInfoTable)(DWORD buffer);
typedef int (WINAPI *GetEmuleStatus)(PDWORD unknow, PDWORD pStatus, PDWORD *ppStatus);
typedef int (*GenerateEd2kLink)(SRESULT result, PVOID pPara);
typedef int (WINAPI *StopKadSearch)(PBYTE pSeed);
typedef int (*GetKadSearchResult)(PBYTE pSeed, PDWORD buffer);
typedef int (WINAPI *StartServerSearch)(char *key, int len);
typedef int (WINAPI *StopServerSearch)(char *key, int len);
typedef int (*GetServerSearchResult)(char *key, int len, PDWORD buffer);
typedef int (WINAPI *StartKadSearch)(char *key, int len, OUT PBYTE pSeed);

#ifdef LOCAL_KAD_LIB
extern KadLibrary g_kadLibrary;
#else
extern Init pInit;
extern Uninit pUninit;
extern DeleteFileBasicInfoTable pDeleteFileBasicInfoTable;
extern GetEmuleStatus pGetEmuleStatus;
extern GenerateEd2kLink pGenerateEd2kLink;
extern StopKadSearch pStopKadSearch;
extern GetKadSearchResult pGetKadSearchResult;
extern StartServerSearch pStartServerSearch;
extern StopServerSearch pStopServerSearch;
extern GetServerSearchResult pGetServerSearchResult;
extern StartKadSearch pStartKadSearch;
#endif

#endif  // COMMON_H

